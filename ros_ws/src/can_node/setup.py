from setuptools import find_packages, setup

package_name = 'can_node'

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
    maintainer='EmeraldWither',
    maintainer_email='68785503+EmeraldWither@users.noreply.github.com',
    description='TODO: Package description',
    license='Apache-2.0',
    tests_require=['pytest'],
    entry_points={
        'console_scripts': [
            "test = can_node.test_data.canmotor_testing_data:main",
            "motor = can_node.can_node:main"
        ],
    },
)
