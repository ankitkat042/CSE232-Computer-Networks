# Assinment 3: Analysis of synchronous and asynchronous TCP socket call APIs
Ankit Kumar 2021015, IIIT Delhi

## Setup Instructions: https://github.com/pnl-iiitd/CSE232-iiitd/tree/main/assignment3 


Graphs and htop results

Fork

500

![](media/image1.png)

![](media/image2.png)

1000

![](media/image3.png)

![](media/image4.png)
3000

![](media/image5.png)

![](media/image6.png)

Thread
500

![](media/image7.png)

![](media/image8.png)

1000

![](media/image9.png)

![](media/image10.png)

3000

![](media/image11.png)

![](media/image12.png)

Epoll\
500

![](media/image13.png)

1000

![](media/image14.png)

3000

![](media/image15.png)

Epoll 500 throughput

![](media/image16.png)

Epoll 1000 Throughput

![](media/image17.png)

Epoll 3000 throughput

![](media/image18.png)

Fork 500 throughput

![](media/image19.png)

Fork 1000

![](media/image20.png)

Fork 3000

![](media/image21.png)

Poll 500

![](media/image22.png)

Poll 1000

![](media/image23.png)

Poll 3000

![](media/image24.png)

Thread Throughput

500

![](media/image25.png)

Thread 1000

![](media/image26.png)

Thread 3000

![](media/image27.png)

SELECT THROUGHPUT
500

![](media/image28.png)

Select 1000

![](media/image29.png)

Fork-based servers:  
- Spawning of new process per connection escalates memory usage due to isolated resource allocation.  
- CPU overhead intensified by increased process count and higher context-switching frequency.  
 
pthread-based servers:  
- Threads within a single process share memory, reducing overall memory footprint compared to process-per-connection model.  
- High thread counts still strain CPU with scheduling overhead. Thread synchronization introduces significant resource overhead.

I/O Multiplexing (select, poll, epoll):
- Single process/thread manages multiple connections, leveraging
asynchronous polling of file descriptors, optimizing memory and CPU
usage.
- select and poll exhibit linear complexity, leading to         decreased efficiency as file descriptor count grows.
- epoll provides near-constant time complexity post-initialization, enhancing performance consistency.


Resource utilization factors:
- Management overhead for each process/thread and their associated data structures.

- CPU load correlates with context switch rates, system call overhead, and computation-intensive operations such as factorial calculations.

- Each bullet point reflects a more precise and technical language suited for an audience familiar with server architectures and performance considerations.