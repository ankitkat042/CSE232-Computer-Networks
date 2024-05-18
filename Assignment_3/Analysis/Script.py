from ctypes import sizeof
import pyshark
import threading
import queue
import os
import matplotlib.pyplot as plt

# Global variables for storing throughput and latency
flows = {}
latencies = {}

# Lock for synchronizing access to the above dictionaries
lock = threading.Lock()

def process_packets(q):
    while True:
        packet = q.get()  # Get a packet from the queue
        if packet is None:
            break  # None is the signal to stop

        try:
            if 'TCP' in packet:
                flow_tuple = (packet.ip.src, packet.ip.dst, packet.tcp.srcport, packet.tcp.dstport)
                length = int(packet.length)
                timestamp = float(packet.sniff_timestamp)

                with lock:
                    # Handle throughput calculation
                    if flow_tuple not in flows:
                        flows[flow_tuple] = {'bytes': 0, 'start_time': timestamp, 'end_time': timestamp}
                    flows[flow_tuple]['bytes'] += length
                    flows[flow_tuple]['end_time'] = timestamp

                    # Handle latency calculation
                    if 'tcp.analysis.ack_rtt' in packet.tcp.field_names:
                        if flow_tuple not in latencies:
                            latencies[flow_tuple] = []
                        latencies[flow_tuple].append(float(packet.tcp.analysis.ack_rtt))

        except AttributeError:
            pass

        q.task_done()  # Mark the task as done

# This function will calculate and write the metrics to a text file.
def calculate_and_save_metrics(output_file):
    throughput_results = {}
    latency_results = {}

    with open(output_file, 'w') as f:
        for flow, data in flows.items():
            duration = data['end_time'] - data['start_time']
            if duration > 0:
                throughput = (data['bytes'] * 8) / duration  # bits per second
                throughput_results[flow] = throughput
                f.write(f"Flow {flow} - Throughput: {throughput:.2f} bits/s\n")

        f.write("\n")  # Empty line to separate the two metrics

        for flow, latency_list in latencies.items():
            if latency_list:
                average_latency = sum(latency_list) / len(latency_list) * 1000  # milliseconds
                latency_results[flow] = average_latency
                f.write(f"Flow {flow} - Average Latency: {average_latency:.2f} ms\n")

    plot_metrics(throughput_results, latency_results)

def plot_metrics(throughput_results, latency_results):
    flows = list(throughput_results.keys())
    throughputs = [throughput_results[flow] for flow in flows]
    latencies = [latency_results.get(flow, 0) for flow in flows]  # Default to 0 if no latency data

    fig, axs = plt.subplots(2, 1, figsize=(10, 8))

    # Throughput plot
    axs[0].bar(range(len(flows)), throughputs, color='blue')
    axs[0].set_title('Average Throughput per TCP Flow')
    axs[0].set_xlabel('TCP Flow Index')
    axs[0].set_ylabel('Throughput (bps)')

    # Latency plot
    axs[1].bar(range(len(flows)), latencies, color='red')
    axs[1].set_title('Average Latency per TCP Flow')
    axs[1].set_xlabel('TCP Flow Index')
    axs[1].set_ylabel('Latency (ms)')

    plt.tight_layout()
    plt.savefig("metrics_plot_select_1000.png")
    plt.show()

def analyze_pcap(pcap_file, num_clients):
    global flows, latencies
    flows.clear()
    latencies.clear()

    cap = pyshark.FileCapture(pcap_file)

    packet_queue = queue.Queue()
    num_threads = 4
    threads = []

    for _ in range(num_threads):
        thread = threading.Thread(target=process_packets, args=(packet_queue,))
        thread.start()
        threads.append(thread)

    for packet in cap:
        packet_queue.put(packet)

    for _ in threads:
        packet_queue.put(None)

    for thread in threads:
        thread.join()

    cap.close()

    # Save the metrics to a file
    base_filename = os.path.splitext(os.path.basename(pcap_file))[0]
    output_filename = f"{base_filename}clients{num_clients}_metrics.txt"
    calculate_and_save_metrics(output_filename)

# Example usage:

# Replace with the actual file paths and number of clients
analyze_pcap('/home/kitkat/Documents/Assignment_3/Analysis/pcapdump/epoll_500.pcap', 500)
# Repeat for other pcap files and number of clients (1000, 1000)