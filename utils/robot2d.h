#ifndef __ROBOT2D_H__
#define __ROBOT2D_H__

#include <isam/isam.h>
#include <vector>
#include <memory>
#include <iomanip>      // std::setw

#include "isam/Anchor.h"

using namespace isam;
using namespace Eigen;

using SlamPtr = std::shared_ptr<Slam>;

class Robot2D
{
public:
  Robot2D(); // single session ver 
  Robot2D(SlamPtr _multi_slam, bool _base_session = false); // multi session ver 

  void initSession();
  void addNode();

  void addOdometryFactor(Pose2d _pose2d_relative);
  void addIntraLoopFactor(int _node_idx_i, int _node_idx_j, Pose2d _pose2d_relative);
  void addInterLoopFactor(int _my_node_idx, Robot2D& _other_robot, int _other_robot_node_idx, Pose2d _pose2d_relative);

  void batchOptimizationMultiSlam(void);
  void batchOptimization(void);

  void saveGraph(std::string _savedir, std::string _name, bool _verbose_local = true, bool _verbose_global = false);

public: // graph optimizer 
  std::shared_ptr<Slam> multi_slam_;
  Slam single_slam_; // for saving robot's independent trajectory

public: // node
  std::shared_ptr<Anchor2d_Node> anchor_node_;

  std::vector<std::shared_ptr<Pose2d_Node> > nodes_;
  std::vector<int> node_idxes_global_;

public: // factors 
  std::shared_ptr<Pose2d_Factor> prior_factor_;
  std::shared_ptr<Pose2d_Factor> anchor_factor_;
  std::vector<std::shared_ptr<Pose2d_Pose2d_Factor> > odometry_factors_;
  std::vector<std::shared_ptr<Pose2d_Pose2d_Factor> > interloop_factors_;
  std::vector<std::shared_ptr<Pose2d_Pose2d_Factor> > intraloop_factors_;

public:
  const Pose2d prior_origin{0., 0., 0.}; // assume a robot always starts at the Origin.

  static Noise Pose2d_Pose2d_noise_;
  static Noise Pose2d_prior_noise_;

public:
  bool flag_multisession_ = false;

  const int this_robot_index_;
  static int curr_robot_index_;
};

#endif // __ROBOT2D_H__
