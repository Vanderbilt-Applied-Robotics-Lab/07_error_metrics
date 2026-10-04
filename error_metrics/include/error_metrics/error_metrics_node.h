#ifndef ERROR_METRICS_NODE
#define ERROR_METRICS_NODE

#include <rclcpp/rclcpp.hpp>
#include "tf2_ros/transform_listener.h"
#include "tf2_ros/buffer.h"
#include <Eigen/Dense>
#include <Eigen/Geometry>


/**
 * Solution for take-home assignment 7 
 * Applied Robotics
 */
class ErrorMetricsNode : public rclcpp::Node
{
public:
    /**
     * Constructor
     */
    ErrorMetricsNode();

    /**
     * Destructor
     */
    ~ErrorMetricsNode() = default;

    /** 
     * Prints the rotation and translation error between the end-effector and both test frames
    */
    void printErrors();
    
private:
    /**
     * Wrapper function for transformLookup(). Will lookup transform between "ee_link" and frame_name
     * @param frame_name the name of the frame to look up
     * @param rot If the lookup is successful, this will get set to the looked up frame's rotation
     * @param tran If the lookup is successful, this will get set to the looked up frame's translation
     * @return True if successful, false otherwise
     */
    bool lookupFrame(const std::string & frame_name, Eigen::Quaterniond & rot, Eigen::Vector3d & tran);


    // Frames
    std::shared_ptr<tf2_ros::TransformListener> tf_listener_;
    std::unique_ptr<tf2_ros::Buffer> tf_buffer_;
    std::array<std::string, 2> frame_names_;
    std::array<bool,2> lookup_success_;

    // error
    std::array<Eigen::Quaterniond,2 > quat_;
    std::array<Eigen::Vector3d,2> pos_;
    std::array<double,2> error_trans_;
    std::array<double,2> error_rot_;
};

#endif // ERROR_METRICS_NODE