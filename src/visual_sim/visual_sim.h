#pragma once

#include <mutex>
#include <ros/ros.h>

// MoveIt
#include <moveit/robot_model_loader/robot_model_loader.h>
#include <moveit/planning_scene/planning_scene.h>
#include <moveit/kinematic_constraints/utils.h>

#include <geometric_shapes/shape_operations.h>
#include <shape_msgs/Mesh.h>

#include <geometric_shapes/shapes.h>
#include <geometric_shapes/bodies.h>
#include <geometric_shapes/shape_operations.h>

#include <fstream>

#include <Eigen/Dense>
#include <unsupported/Eigen/CXX11/Tensor>

#include <pcl_ros/point_cloud.h>
#include <gl_depth_sim/sim_depth_camera.h>
#include <gl_depth_sim/interfaces/pcl_interface.h>

#include "eigen_tools/eigen_tools.h"

typedef pcl::PointCloud<pcl::PointXYZ> CloudXYZ;
typedef std::shared_ptr<pcl::PointCloud<pcl::PointXYZ> > CloudXYZPtr;
namespace gds = gl_depth_sim;

class VisualSim
{
public:
  VisualSim(int width, int height, double fx, double fy, double z_near, double z_far);

  void setCamPos(const Eigen::Ref<const Eigen::Vector3d> &pos);
  void setCamPose(const Eigen::Isometry3d &cam_pose);
  void lookat(const Eigen::Ref<const Eigen::Vector3d> &target);

  void loadScene(const planning_scene::PlanningScenePtr & scene);
  CloudXYZPtr generatePointCloud();
  Eigen::MatrixXd generatePointCloudMatrix();
  Eigen::VectorXi generateLocalVoxelOccupancy(const Eigen::MatrixXd &point_cloud_matrix, 
                                              const Eigen::Isometry3d& obj_pose,
                                              const Eigen::Ref<const Eigen::Vector3d> &cam_pos,
                                              const Eigen::Ref<const Eigen::Vector3d> &obj_bound_min,
                                              const Eigen::Ref<const Eigen::Vector3d> &obj_bound_max,
                                              const Eigen::Ref<const Eigen::Vector3d> &n_grids, 
                                              double near_distance,
                                              bool fill_occluded_voxels);
  Eigen::VectorXi generateVoxelOccupancy();
  Eigen::MatrixXf generateDepthImage();

  gds::Mesh geomToMesh(const shapes::ShapeConstPtr &shape, 
                       const std::string &name);
  
  void setGridResolution(const int n_grid);
  void setGridResolutions(const int n_grid_x, const int n_grid_y, const int n_grid_z);
  void setSceneBounds(const Eigen::Ref<const Eigen::Vector3d> &scene_bound_min,
                      const Eigen::Ref<const Eigen::Vector3d> &scene_bound_max);
  
private:
  // Depth camera
  gds::CameraProperties cam_props_;
  Eigen::Isometry3d cam_pose_;
  std::shared_ptr<gds::SimDepthCamera> sim_;
  
  // Voxel occupancy grid
  std::pair<Eigen::Vector3d,Eigen::Vector3d> occupancy_bound_;

  int n_grid_ {16};

  int n_grids_[3];
  double length_arr_[3];
  double resolutions_[3];

  Eigen::Tensor<int, 3> voxel_grid;
};