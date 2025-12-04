from rclpy import QoSProfile
from rclpy.service_introspection import ServiceIntrospectionState

import json
import os
import rclpy
from rcl_interfaces.msg import ParameterDescriptor, ParameterType
from rclpy.node import Node
from rclpy.executors import ExternalShutdownException
from boat_data_interfaces.msg import BoatAlarm
from boat_data_interfaces.msg import ShoreBoatAlarm

import csv

from boat_data_interfaces.srv import AlarmRaise, AlarmDelatch


class AlarmsWatchdog(Node):
    def __init__(self):
        super().__init__('alarms_watchdog')
        description = ParameterDescriptor(description='Defines where to find the exact file for the fault codes csv')
        self.declare_parameter('faults_file', '~/eboat_src/data/FAULTS.csv', description)
        self.declare_parameter('replay_mode', False, ParameterDescriptor(description="Sets the node into a replay mode, where it doesn't send out data to the shore."))

        # Error Code, Type, Message
        self.codes = {}
        self.load_csv_file()
        self.raised_sticky_alarms = set({})


        a = self.create_service(AlarmRaise, "/alarm/raise", self.on_alarm_raise)
        b = self.create_service(AlarmDelatch, "/alarm/delatch", self.on_alarm_delatch)
        # Service introspection is needed for us to be
        a.configure_introspection(self._clock, service_event_qos_profile=QoSProfile(depth=10), introspection_state=ServiceIntrospectionState.CONTENTS)
        b.configure_introspection(self._clock, service_event_qos_profile=QoSProfile(depth=10), introspection_state=ServiceIntrospectionState.CONTENTS)


        self.shore_pub = self.create_publisher(ShoreBoatAlarm, "/alarm/shore/publish", 10)

        if bool(self.get_parameter("replay_mode").get_parameter_value().bool_value):
            self._logger.info("Watchdog node is in replay mode!")


    def on_alarm_raise(self, request:AlarmRaise.Request, response: AlarmRaise.Response) -> AlarmRaise.Response:
        alarm = request.alarm
        if not self.codes.__contains__(alarm.error_code):
            self._logger.error("An unknown alarm was raised with error code " + str(alarm.error_code))
            response.result = AlarmRaise.Response.UNKNOWN
            return response

        error_code = alarm.error_code
        error_type = self.codes[alarm.error_code][0]
        error_message = self.codes[alarm.error_code][1]
        sticky = self.codes[alarm.error_code][3]
        if not sticky:
            if error_type == "FAULT":
                self._logger.error("Alarm was raised:\n\tError Code: " + str(
                    error_code) + "\n\tDescription: " + error_message + "\n\tSeverity: " + error_type)
            else:
                self._logger.warn("Alarm was raised:\n\tError Code: " + str(
                    error_code) + "\tDescription: " + error_message + "\tSeverity: " + error_type)

        already_raised = self.raised_sticky_alarms.__contains__(error_code)
        if sticky and already_raised:
            self._logger.info(f"Alarm ID {error_code} is sticky, and has already been raised")
            response.result = AlarmRaise.Response.STICKY_ALREADY_RAISED
            return response
        elif sticky and not already_raised:
            if error_type == "FAULT":
                self._logger.error("Alarm was raised:\n\tError Code: " + str(
                    error_code) + "\n\tDescription: " + error_message + "\n\tSeverity: " + error_type)
            else:
                self._logger.warn("Alarm was raised:\n\tError Code: " + str(
                    error_code) + "\tDescription: " + error_message + "\tSeverity: " + error_type)
            response.result = AlarmRaise.Response.STICKY_NOT_RAISED_BEFORE
            self.raised_sticky_alarms.add(error_code)
        else:
            response.result = AlarmRaise.Response.RAISED
        if not bool(self.get_parameter("replay_mode").get_parameter_value().bool_value):
            shoreAlarm = ShoreBoatAlarm()
            shoreAlarm.error_code = alarm.error_code
            shoreAlarm.message = error_message
            shoreAlarm.timestamp = alarm.timestamp
            self.shore_pub.publish(shoreAlarm)
        return response

    def on_alarm_delatch(self, request:AlarmDelatch.Request, response: AlarmDelatch.Response) -> AlarmDelatch.Response:
        code = request.error_code
        error_message = self.codes[code][1]
        error_type = self.codes[code][0]

        if self.raised_sticky_alarms.__contains__(code):
            self._logger.info(f"Alarm with id {code} was UNLATCHED. \n\tDescription: {error_message} \n\tSeverity: {error_type}")
            self.raised_sticky_alarms.remove(code)
            response.success = True
        else:
            response.success = False

        return response



    def load_csv_file(self):
        file_path = self.get_parameter('faults_file').get_parameter_value().string_value
        file_path = os.path.expanduser(file_path)
        self._logger.info("Attempting to load " + file_path)
        with open(file_path, 'r') as file:
            reader = csv.DictReader(file)
            i = 0
            for row in reader:
                if len(row["ID"]) == 0:
                    continue
                self.codes[int(row["ID"])] = (row['Type'], row['Message'], row['Condition'], row['Sticky'])
                i += 1
        
        self._logger.info("Loaded " + str(i) + " error codes from faults.csv")
        self._logger.info("All Error Codes: " + json.dumps(self.codes, indent=2))



def main(args=None):
    try:
        with rclpy.init(args=args):
            rclpy.spin(AlarmsWatchdog())
    except (KeyboardInterrupt, ExternalShutdownException):
        pass


if __name__ == '__main__':
    print("Starting!!")
    main()
