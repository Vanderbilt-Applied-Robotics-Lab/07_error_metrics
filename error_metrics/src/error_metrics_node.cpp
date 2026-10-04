#include <error_metrics/error_metrics_node.h>

ErrorMetricsNode::ErrorMetricsNode() : Node("error_metrics_node")
{       
    // frame names
    frame_names_[0] = "target_frame_1";
    frame_names_[1] = "target_frame_2";
    

    // setup transform buffer
    tf_buffer_ = std::make_unique<tf2_ros::Buffer>(this->get_clock());
    tf_listener_ = std::make_shared<tf2_ros::TransformListener>(*tf_buffer_);
}

void ErrorMetricsNode::printErrors()
{
    // loop through each frame
    for (uint8_t i = 0; i < frame_names_.size(); ++i)
    {
        lookup_success_[i] = lookupFrame(frame_names_[i], quat_[i], pos_[i]);
        
        // only compute metrics if lookup succeeded 
        if(lookup_success_[i])
        {
            // Convert quat to axis angle
            Eigen::AngleAxisd ax_ang = Eigen::AngleAxisd(quat_[i]);

            //rotation error
             error_rot_[i] = ax_ang.angle()*180.0/M_PI; // deg

            // translation error
            error_trans_[i] = pos_[i].norm();
        }
    }

    // only print if BOTH succeeded 
    if (lookup_success_[0] && lookup_success_[1])
    {
        // rotation
        uint8_t min_i_rot = std::distance(error_rot_.begin(), std::min_element(error_rot_.begin(), error_rot_.end())); // fancy way of getting the index of the min element in an array
        RCLCPP_INFO(this->get_logger(), "Closer to frame %s in rotation. Error: %0.5f deg", frame_names_[min_i_rot].data(), error_rot_[min_i_rot]);

                // rotation
        uint8_t min_i_tran = std::distance(error_trans_.begin(), std::min_element(error_trans_.begin(), error_trans_.end())); // fancy way of getting the index of the min element in an array
        RCLCPP_INFO(this->get_logger(), "Closer to frame %s in translation. Error: %0.5f m", frame_names_[min_i_tran].data(), error_trans_[min_i_tran]);
    }
}

bool ErrorMetricsNode::lookupFrame(const std::string & frame_name, Eigen::Quaterniond & rot, Eigen::Vector3d & tran)
{
    try
    {
        geometry_msgs::msg::TransformStamped transform = tf_buffer_->lookupTransform("ee_link", frame_name, tf2::TimePointZero);
        
        // set translation of output frame
        tran << transform.transform.translation.x, transform.transform.translation.y, transform.transform.translation.z;

        // set translation of output frame
        rot =  Eigen::Quaterniond(
            transform.transform.rotation.w, 
            transform.transform.rotation.x, 
            transform.transform.rotation.y,
            transform.transform.rotation.z
        );

        RCLCPP_INFO(this->get_logger(), "Found frame: %s", frame_name.data());
        
        return true;
    }
    catch (const tf2::TransformException & ex)
    {
        RCLCPP_ERROR(this->get_logger(), "lookup failed! Reason: %s", ex.what());

        return false;
    }
}

int main(int argc, char** argv)
{
    // initialize the node
    rclcpp::init(argc, argv);

    // create instance of class
    auto node = std::make_shared<ErrorMetricsNode>();

    // Set loop rate
    rclcpp::Rate rate = rclcpp::Rate(1); // Hz
    
    while (rclcpp::ok())
    {
        rclcpp::spin_some(node); // allow the /robot_description callback to fire first
        
        // break if there are not any new poses to solve
        node->printErrors();

        rate.sleep();
    }
    rclcpp::shutdown();
    return 0;
}