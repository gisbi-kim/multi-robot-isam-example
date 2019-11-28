/**
 * @brief A useful robot trajectory class to employ Anchor node 
 * @author Giseop Kim (paulgkim@kaist.ac.kr)
 */
#include <cmath>
#include <vector>
#include <memory>

#include <isam/isam.h>
#include <isam/Anchor.h>

#include <robot2d.h> // == #include "utils/robot2d.h"

using std::cout;
using std::endl;
using std::string;

#define lc cout << endl // line change

const double angle_turn_left{(M_PI/2.0)}; // 90 deg = pi/2
const double one_step{1.0};

auto main() -> int
{
  // Init parameters 
  const float information_val{10.0}; 
  Robot2D::Pose2d_Pose2d_noise_ = SqrtInformation(information_val * eye(3));

  // Init a global slam optimizer (each robot has this) 
  auto multislam = std::make_shared<Slam>(); // required to be managed using smart pointer in this exmaple
  std::vector<Robot2D*> multi_robots; // save pointers

  // First, robot 1 is generated and starts to operate 
  bool is_base_session = true;
  Robot2D robot_1(multislam, is_base_session); // multi sesssion ver 
  multi_robots.push_back(&robot_1); 
  is_base_session = false; // off the flag (because anchor graph also has the only prior)

  // The robot 1 moves a single step in x  
  robot_1.addOdometryFactor(Pose2d(one_step, 0.0, 0.0));

  // The robot 1 moves again 2 steps more in x and 1 step in y  
  robot_1.addOdometryFactor(Pose2d(2.0*one_step, one_step, 0.0));

  // While the robot 1 is moving, 
  // A robot 2, the other robot, is generated and starts to operate 
  Robot2D robot_2(multislam);
  multi_robots.push_back(&robot_2); 

  // Meanwhile, the robot 1 moves again a single step in x.  
  robot_1.addOdometryFactor(Pose2d(one_step, 0.0, 0.0));

  // The robot 2 moves 3 steps in x 
  robot_2.addOdometryFactor(Pose2d(2.0*one_step, 0.0, 0.0));

  // The robot 1 and 2 encountered (i.e., inter-loop detected)
  // - Assume the node 1 of the robot 1 and the node 0 of the robot 2 has a relative transformation (1,1,0)
  int loop_node_target;
  int loop_node_matched;
  Pose2d loop_relative;

  loop_node_target = 1;
  loop_node_matched = 0;
  loop_relative = Pose2d(1., 1., 0.);
  robot_1.addInterLoopFactor(loop_node_target, robot_2, loop_node_matched, loop_relative);

  // optimization including the anchor node graph
  cout << endl << "optimize the multi-session graph ..." << endl;
  multislam->batch_optimization(); // == robot_1.batchOptimizationMultiSlam()

  // save the optimized graph
  bool verbose_local{true};
  bool verbose_global{true};
  std::string save_dir{"./"};
  std::string save_name;
  cout << endl << "----- The optimized graph -----" << endl;
  for(auto _robot_ptr: multi_robots)
  {
    cout << "Robot " << _robot_ptr->this_robot_index_ << " graph:" << endl;
    save_name = "robot_" + std::to_string(_robot_ptr->this_robot_index_) + "_opt_1";
    _robot_ptr->saveGraph(save_dir, save_name, verbose_local, verbose_global); cout << endl;
  }

  // While the robot 1 and 2 are going around, 
  // The other robot, a robot 3 is generated and starts to operate 
  Robot2D robot_3(multislam); 
  multi_robots.push_back(&robot_3); 

  // The robot 3 moves -1 steps in x, 1 step in y, and rotates 90 deg 
  robot_3.addOdometryFactor(Pose2d( -1.0*one_step, one_step, angle_turn_left));

  // Meanwhile, the robot 2 moves again a single step in x.  
  robot_2.addOdometryFactor(Pose2d(one_step, 0.0, 0.0));

  // The robot 2 and 3 encountered (i.e., inter-loop detected)
  // - Assume the node 0 of the robot 2 and the node 1 of the robot 3 has a relative transformation (0,1,0)
  loop_node_target = 0;
  loop_node_matched = 1;
  loop_relative = Pose2d(0., 1., 0.);
  robot_2.addInterLoopFactor(loop_node_target, robot_3, loop_node_matched, loop_relative);

  // optimization including the anchor node graph
  cout << endl << "optimize the multi-session graph ..." << endl;
  multislam->batch_optimization(); // == robot_2.batchOptimizationMultiSlam()
  
  // save again 
  cout << endl << "----- The optimized graph -----" << endl;
  for(auto _robot_ptr: multi_robots)
  {
    cout << "Robot " << _robot_ptr->this_robot_index_ << " graph:" << endl;
    save_name = "robot_" + std::to_string(_robot_ptr->this_robot_index_) + "_opt_2";
    _robot_ptr->saveGraph(save_dir, save_name, verbose_local, verbose_global); cout << endl;
  }

  return 0;
}
