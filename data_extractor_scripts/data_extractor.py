import sys
from datetime import datetime
from zoneinfo import ZoneInfo

import pandas as pd
from mcap_ros2.reader import read_ros2_messages


def extract(topics, bag_file, csv_file):
    # TODO: add ros bag time
    records = []
    ros_topics = list(map(lambda x: x.split(".")[0], topics))
    for msg in read_ros2_messages(bag_file, topics=ros_topics):
        ros_msg = msg.ros_msg
        for attr in topics:
            topic = attr.split(".")[0]
            topic_data = attr.split(".")[1]
            if not msg.channel.topic == topic:
                continue

            data = getattr(ros_msg, topic_data, None)
            if data is not None:
                ts_ns = msg.publish_time_ns
                ts_sec = ts_ns / 1e9
                ts_str = datetime.fromtimestamp(ts_sec, tz=ZoneInfo("America/New_York")).strftime(
                    "%m/%d/%y | %I:%M:%S %p")

                records.append({
                    "timestamp_ns": ts_ns,
                    "timestamp_sec": ts_sec,
                    "timestamp": str(ts_str),
                    attr: data,
                })

    # Build DataFrame
    df = pd.DataFrame(records)

    if df.empty:
        print("⚠️ No relevant messages found.")
    else:
        # Floor timestamps to the nearest second
        df["timestamp_sec_floor"] = df["timestamp_sec"].astype(int)

        # Group by floored second and take the mean of each value
        agg = {
            "timestamp": "first"
        }
        for attr in input_topics:
            agg[attr] = "mean"
        grouped = df.groupby("timestamp_sec_floor").agg(agg).reset_index()
        grouped.to_csv(csv_file, index=False)

        print(f"Wrote {len(grouped)} grouped records to {output_csv_file}")


if __name__ == "__main__":
    if len(sys.argv) > 3:
        input_topics = set(sys.argv[1].split(","))
        input_ros_bag_file = sys.argv[2]
        output_csv_file = sys.argv[3]
        print(f"Using the topics {input_topics} with an input .mcap file of {input_ros_bag_file} outputting a file at {output_csv_file}.")
        extract(input_topics, input_ros_bag_file, output_csv_file)
    else:
        print("Please provide the following parameters")
        print(
            """--> A comma seperated list of input topics with their data (e.g '/electrical/temp_sensors/out.outlet_temp'. Keep in mind that you need to have it in the format of {topic name}.{name of data/variable}""")
        print("""--> The input .mcap file that is inside your rosbag folder.""")
        print("""--> The name of the output .csv file for your data""")
        print("ex: python3 data_extractor.py /electrical/temp_sensors/out.outlet_temp,/electrical/temp_sensors/in.inlet_temp ros_bag_2.mcap 7_11_23-temp-aanalysis.csv")
        exit(-1)
