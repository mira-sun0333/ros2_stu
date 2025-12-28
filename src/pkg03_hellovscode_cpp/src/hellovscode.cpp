#include "rclcpp/rclcpp.hpp"

int main(int argc,char const *argv[])
{
  rclcpp::init(argc,argv);
  auto node = rclcpp::Node::make_shared("hellovscode_node_cpp");

  RCLCPP_INFO(node->get_logger(),"hello vscode.......");

  rclcpp::shutdown();
  return 0;





}