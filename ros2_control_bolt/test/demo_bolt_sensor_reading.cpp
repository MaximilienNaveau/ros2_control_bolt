/* Utile pour la lecture de capteur ---> read() de system_bolt.cpp*/
/* Inspiré de demo_create_bolt_robot.cpp*/



#include <system_bolt.hpp>

#include <odri_control_interface/utils.hpp>
#include <odri_control_interface/imu.hpp>


using namespace odri_control_interface;

#include <iostream>
#include <stdexcept>

typedef Eigen::Matrix<double, 6, 1> Vector6d;
typedef Eigen::Matrix<bool, 6, 1> Vector6b;

typedef Eigen::Matrix<long, 3, 1> Vector3l;
typedef Eigen::Matrix<long, 4, 1> Vector4l;
typedef Eigen::Matrix<long, 6, 1> Vector6l;
typedef Eigen::Matrix<int, 6, 1> Vector6i;

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        throw std::runtime_error(
            "Please provide the interface name "
            "(i.e. using 'ifconfig' on linux");
    }

    nice(-20);  // Give the process a high priority.

    auto main_board_ptr_ = std::make_shared<MasterBoardInterface>(argv[1]);

    Vector6i motor_numbers;
    motor_numbers << 0, 3, 2, 1, 5, 4;
    Vector6b motor_reversed;
    motor_reversed << true, false, true, true, false, false;

    Vector6d joint_lower_limits;
    joint_lower_limits << -0.5, -1.7, -3.4, -0.5, -1.7, -3.4;     //Modif d'après lecture des capteurs (demo bolt)
    Vector6d joint_upper_limits;
    joint_upper_limits << 0.5, 1.7, +3.4, +0.5, +1.7, +3.4;       //Modif d'après lecture des capteurs (demo bolt)

    // Define the joint module.
    auto joints = std::make_shared<JointModules>(main_board_ptr_,
                                                 motor_numbers,
                                                 0.025,
                                                 9.,
                                                 12.,	  //MAX CURRENT = 12
                                                 motor_reversed,
                                                 joint_lower_limits,
                                                 joint_upper_limits,
                                                 80.,
                                                 0.5);

    // Define the IMU.
    Vector3l rotate_vector;
    Vector4l orientation_vector;
    rotate_vector << 1, 2, 3;
    orientation_vector << 1, 2, 3, 4;
    auto imu = std::make_shared<IMU>(
        main_board_ptr_, rotate_vector, orientation_vector);

    // Define the robot.
    auto robot = std::make_shared<Robot>(main_board_ptr_, joints, imu);

    // Start the robot.
    robot->Start();


    int c = 0;
    std::chrono::time_point<std::chrono::system_clock> last =
        std::chrono::system_clock::now();
    while (!robot->IsTimeout())
    {
        if (((std::chrono::duration<double>)(std::chrono::system_clock::now()-last)).count() > 0.001)
        {
            last = std::chrono::system_clock::now();  // last+dt would be better
            robot->ParseSensorData();

            c++;
            if (c % 1000 == 0)
            {
                std::cout << "Count :                        " << c << "\n";
                std::cout << "\n";

                std::cout << "Joints : \n";
                std::cout << "Position:                      ";
                joints->PrintVector(joints->GetPositions());
                std::cout << "\n";
                std::cout << "Velocities:                    ";
                joints->PrintVector(joints->GetVelocities());
                std::cout << "\n";
                std::cout << "Measured Torques:              ";
                joints->PrintVector(joints->GetMeasuredTorques());
                std::cout << "\n";
                std::cout << "\n";

                std::cout << "IMU : \n";
                std::cout << "Gyroscope                      ";
                joints->PrintVector(imu->GetGyroscope());
                std::cout << "\n";
                std::cout << "Accelerometer                  ";
                joints->PrintVector(imu->GetAccelerometer());
                std::cout << "\n";
                std::cout << "Linear Acceleration            ";
                joints->PrintVector(imu->GetLinearAcceleration());
                std::cout << "\n";
                std::cout << "Attitude Euler                 ";
                joints->PrintVector(imu->GetAttitudeEuler());
                std::cout << "\n";
                std::cout << "Attitude Quaternion            ";
                joints->PrintVector(imu->GetAttitudeQuaternion());
                std::cout << "\n";
                std::cout << "\n";
                std::cout << "\n";
                std::cout << "\n";
                std::cout << std::endl;
                
            }
        }
        else
        {
            std::this_thread::yield();
        }
    }
    std::cout << "Normal program shutdown." << std::endl;
    return 0;
}

