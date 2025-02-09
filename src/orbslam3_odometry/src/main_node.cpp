#include <iostream>
#include <algorithm>
#include <fstream>
#include <chrono>

#include "rclcpp/rclcpp.hpp"
#include "stereo/stereo.hpp" //  TODO forse metti in include
#include "System.h"
#include <signal.h>

#define DEBUG true

void handleSignal(int signal)
{
    if (signal == SIGINT)
    {
        std::cout << "Received SIGINT. Killing MainNode process.\n";
        rclcpp::shutdown();
    }
}

class MainNode : public rclcpp::Node
{
public:
    MainNode() : Node("main_node")
    {
        /* ***** DECLARING PARAMETERS ***** */

        this->declare_parameter("pangolin_visualization", false);
        this->declare_parameter("path_vocabulary", "");
        this->declare_parameter("path_yaml_settings", "");
        this->declare_parameter("system_mode", "");
        this->declare_parameter("just_check_stereo_calibration", false);
        this->declare_parameter("just_take_picture", false);

        /* ******************************** */

        /* ***** READING PARAMETERS ***** */

        this->get_parameter("pangolin_visualization", pangolin_visualization);
        this->get_parameter("path_vocabulary", path_vocabulary);
        this->get_parameter("path_yaml_settings", path_settings);
        this->get_parameter("system_mode", system_mode);
        this->get_parameter("just_check_stereo_calibration", just_check_stereo_calibration);
        this->get_parameter("just_take_picture", just_take_picture);

        std::cout << "Sono nella classe " << std::endl;

        if (DEBUG)
            printDebug();


        RCLCPP_INFO(this->get_logger(), "Initializing stereo");
        ORB_SLAM3::System pSLAM(path_vocabulary, path_settings, ORB_SLAM3::System::STEREO, pangolin_visualization);
        RCLCPP_INFO(this->get_logger(), "after Initializing stereo");
        auto node = std::make_shared<StereoSlamNode>(&pSLAM, path_settings);
        rclcpp::spin(node);
        return;
        
    }

private:
    bool pangolin_visualization, just_check_stereo_calibration, just_take_picture;
    std::string path_vocabulary, path_settings, system_mode;

    void printDebug()
    {
        RCLCPP_INFO(this->get_logger(), "ORB Voc %s", path_vocabulary.c_str());
        RCLCPP_INFO(this->get_logger(), "Settings %s", path_settings.c_str());
        RCLCPP_INFO(this->get_logger(), "Pangolin: %s", pangolin_visualization ? "true" : "false");
        RCLCPP_INFO(this->get_logger(), "System mode: %s", system_mode.c_str());
    }
};

int main(int argc, char **argv)
{
    signal(SIGINT, handleSignal);

    /* node initialization */
    rclcpp::init(argc, argv);

    auto node = std::make_shared<MainNode>();

    std::cout << "============================ " << std::endl;

    rclcpp::spin(node);
    rclcpp::shutdown();

    return 0;
}
