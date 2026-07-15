from setuptools import find_packages, setup

package_name = 'arduinobot_controller'

setup(
    name=package_name,
    version='0.0.0',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='control',
    maintainer_email='andresaleman142@gmail.com',
    description='ROS 2 controller package for the Arduinobot robot arm. Provides a ros2_control hardware interface for serial communication with an Arduino, joint trajectory controllers for the arm and gripper, and nodes for forwarding joint commands from GUI sliders to the controllers.',
    license='Apache-2.0',
    extras_require={
        'test': [
            'pytest',
        ],
    },
    entry_points={
        'console_scripts': [
        'slider_control = arduinobot_controller.slider_control:main',
        ],
    },
)
