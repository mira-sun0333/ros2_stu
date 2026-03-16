from setuptools import find_packages, setup
import os
from glob import glob

package_name = 'py05_exercise'

setup(
    name=package_name,
    version='0.0.0',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
        ('share/' + package_name, glob("launch/*.launch.py")),
        ('share/' + package_name, glob("launch/*.launch.xml"))


    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='abc',
    maintainer_email='abc@todo.todo',
    description='TODO: Package description',
    license='TODO: License declaration',
    tests_require=['pytest'],
    entry_points={
        'console_scripts': [
            'exer1_spawn_py = py05_exercise.exer1_spawn_py:main',
            'exer2_tf_broadcaster_py = py05_exercise.exer2_tf_broadcaster_py:main',
            'exer3_tf_listener_py = py05_exercise.exer3_tf_listener_py:main'
        ],
    },
)
