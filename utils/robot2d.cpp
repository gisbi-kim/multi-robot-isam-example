#include <robot2d.h>

// initialize static vars
Noise Robot2D::Pose2d_Pose2d_noise_ = SqrtInformation(10. * eye(3)); // default information 
Noise Robot2D::Pose2d_prior_noise_ = SqrtInformation(10000. * eye(3)); // guarantee the origin is strongly fixed

int Robot2D::curr_robot_index_ = 1; // robot index starts from 1


Robot2D::Robot2D()
:this_robot_index_(curr_robot_index_)
{
    flag_multisession_ = false; 
    initSession();

    // update robot index 
    curr_robot_index_ = curr_robot_index_+ 1;
} // ctor for single slam


Robot2D::Robot2D(SlamPtr _multi_slam, bool _base_session)
:this_robot_index_(curr_robot_index_)
{
    // set the flag 
    flag_multisession_ = true;

    // init global slam manager 
    multi_slam_ = _multi_slam;
    anchor_node_ = std::make_shared<Anchor2d_Node>(multi_slam_.get());
    multi_slam_->add_node(anchor_node_.get());

    // init anchor graph's piror (only once at first initialized)
    if(_base_session) 
    {
      anchor_factor_ = std::make_shared<Pose2d_Factor>(anchor_node_.get(), prior_origin, Pose2d_prior_noise_);
      multi_slam_->add_factor(anchor_factor_.get()); // get means getting the actual address of the smart pointer 
    }

    // init the graph 
    initSession();

    // update robot index 
    std::cout << "Robot " << this_robot_index_ << " is generated." << std::endl;
    curr_robot_index_ = curr_robot_index_+ 1;
} // ctor for multi slam


void 
Robot2D::initSession()
{
    // init node 
    addNode();

    // init factor (prior)
    int node_idx_origin{0};
    prior_factor_ = std::make_shared<Pose2d_Factor>(nodes_.at(node_idx_origin).get(), prior_origin, Pose2d_prior_noise_);

    // add to the graph 
    single_slam_.add_factor(prior_factor_.get());
    if(flag_multisession_)
        multi_slam_->add_factor(prior_factor_.get());
} // Robot2D::initSession


void 
Robot2D::addNode()
{
    auto pose = std::make_shared<Pose2d_Node>();
    nodes_.push_back(std::move(pose)); // because cannot copy unqptr, so move

    single_slam_.add_node(nodes_.back().get()); // == nodes_.back()
    if(flag_multisession_)
        multi_slam_->add_node(nodes_.back().get()); // get means getting the actual address of the smart pointer 

    if(flag_multisession_)
        // get current session's node index within the overall global slam graph 
        node_idxes_global_.push_back(multi_slam_->num_nodes()-1);
    else
        node_idxes_global_.push_back(single_slam_.num_nodes()-1);

} // Robot2D::addNode


void 
Robot2D::addIntraLoopFactor(int _node_idx_i, int _node_idx_j, Pose2d _pose2d_relative)
{
    assert( _node_idx_i < nodes_.size() && _node_idx_j < nodes_.size() );

    // make loop factor
    auto intraloop_factor = std::make_shared<Pose2d_Pose2d_Factor>(
          nodes_.at(_node_idx_i).get(), // my node's ptr
          nodes_.at(_node_idx_j).get(), // query node's ptr
          _pose2d_relative, // constraint 
          Pose2d_Pose2d_noise_
        );
    // save locally (to prevent interloop_factor is deleted after this block)
    intraloop_factors_.push_back(move(intraloop_factor));

    // add the loop factor
    single_slam_.add_factor(intraloop_factors_.back().get());
    if(flag_multisession_)
        multi_slam_->add_factor(intraloop_factors_.back().get()); 

    // no need to add to the single session slam 

} // Robot2D::addIntraLoopFactor


void 
Robot2D::addOdometryFactor(Pose2d _pose2d_relative)
{
    // first add a new node 
    addNode();
    
    // add a factor btn curr and new node 
    int node_idx_curr = nodes_.size()-2;
    int node_idx_new_inserted = node_idx_curr + 1;

    std::cout << "Odometry [Robot " << this_robot_index_ << "]: " 
         << node_idx_curr << " (" << node_idxes_global_.at(node_idx_curr) << " in global) <-> " 
         << node_idx_new_inserted << " (" << node_idxes_global_.back() << " in global)" << std::endl;

    Pose2d odo(_pose2d_relative);
    auto odo_factor = std::make_shared<Pose2d_Pose2d_Factor>(
          nodes_.at(node_idx_curr).get(), // prv node ptr
          nodes_.at(node_idx_new_inserted).get(), // a new node ptr
          odo, // relative constraint 
          Pose2d_Pose2d_noise_ // noise 
        );
    odometry_factors_.push_back(move(odo_factor));

    single_slam_.add_factor(odometry_factors_.back().get()); 
    if(flag_multisession_)
        multi_slam_->add_factor(odometry_factors_.back().get()); 
} // Robot2D::addOdometryFactor


void 
Robot2D::addInterLoopFactor(int _my_node_idx, 
                            Robot2D& _other_robot, int _other_robot_node_idx, 
                            Pose2d _pose2d_relative)
{
    assert(flag_multisession_ == true);

    std::cout << "Inter-session loop: "
         << "[Robot " << this_robot_index_ << "]: " 
         << _my_node_idx << "(" << node_idxes_global_.at(_my_node_idx) << " in global) <-> " 
         << "[Robot " << _other_robot.this_robot_index_ << "]: " 
         << _other_robot_node_idx << "(" << _other_robot.node_idxes_global_.at(_other_robot_node_idx) << " in global)" << std::endl;

    // make loop factor
    auto interloop_factor = std::make_shared<Pose2d_Pose2d_Factor>(
          nodes_.at(_my_node_idx).get(), // my node's ptr
          _other_robot.nodes_.at(_other_robot_node_idx).get(), // query node's ptr
          _pose2d_relative, // constraint 
          Pose2d_Pose2d_noise_, // noise
          anchor_node_.get(), // my anchor
          _other_robot.anchor_node_.get() // query anchor 
        );
    // save locally (to prevent interloop_factor is deleted after this block)
    interloop_factors_.push_back(move(interloop_factor));

    // add the loop factor
    multi_slam_->add_factor(interloop_factors_.back().get());

    // no need to add to the single session slam 

} // Robot2D::addInterLoopFactor


void 
Robot2D::batchOptimizationMultiSlam(void)
{
    if(flag_multisession_)
        multi_slam_->batch_optimization(); //updates all robots' poses 
    // single_slam_.batch_optimization(); // this is included in the above process, and vice versa
} // Robot2D::batchOptimizationMultiSlam


void 
Robot2D::batchOptimization(void) 
{
    single_slam_.batch_optimization();
} // Robot2D::batchOptimization


void 
Robot2D::saveGraph(std::string _savedir, std::string _name, bool _verbose_local, bool _verbose_global) const
{
    std::string savedir{_savedir};
    std::string end_str{_savedir.back()};
    if(end_str.compare(std::string{"/"}))
        savedir = savedir + "/";
    std::string save_name{savedir + _name + ".graph"};

    single_slam_.save(save_name);
    if(_verbose_local)
    {
        std::cout << "  anchor transform wrt the global: " << anchor_node_->value() << std::endl;
        for (int i=0; i<nodes_.size(); ++i)
        {
            // disp local coord
            std::cout << "  x(" << i << "): robot frame " << nodes_.at(i).get()->value() << " "; // << std::endl;

            // disp global coord (transformed using anchor node's relative)
            if(_verbose_global)                
                std::cout << "/ global frame " << anchor_node_->value().oplus(nodes_.at(i).get()->value()) << std::endl;
            else
                std::cout << std::endl;
        }
    }

    std::cout << " graph saved as: " << save_name << std::endl;

    // save the overall multi-session graph also
    multi_slam_->save(savedir + "multisession_slam.graph");
} // Robot2D::saveGraph

