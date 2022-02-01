# data_parsing_and_notify_with_timestamps
�� ������Ʈ�� ADTF(Automotive Data and Time-Triggered Framework) DAT�� <BR>
������� Streaming�����͸� �Ľ��Ͽ� �Ͽ�����, <BR>
TimeStamp�� �°� notify�ϵ��� ����� ���Ҵ�.<BR>
�߰������� �������� notify�� ����� ������ ����ϵ��� �����Ϸ��Ѵ�.<BR>

In this project, I tried to parse and report the streaming data made <BR>
with ADTF (Automotive Data and Time-Triggered Framework) DAT, <BR> 
and to notify according to TimeStamp. <BR>
Additionally, we are trying to support outputting an image <BR>
whether a normal notification has been reached. <BR>

##  Function
1. ADTF DAT parsing streaming-data using ADTFStreaming library
2. Gathering streaming-data with timestamp
3. Scheduling and transport on time (thread)
4. Rendering streaming-data using opencv (thread) - not yet

## License
�� ������Ʈ�� ����ġ 2.0 ���̼����� ������ �ִ�. �׸��� adtf streaming library(MPL-2.0)�� opencv(BSD-3-Clause)�� �� ����ϰ� �ִ�.

This project is under the Apache 2.0 license. And adtf streaming library (MPL-2.0) and opencv (BSD-3-Clause) are used.

# Reference and code
https://www.elektrobit.com/products/automated-driving/eb-assist/adtf/
https://support.digitalwerk.net/adtf_libraries/adtf-streaming-library/v2/api/page_whats_new.html
https://opencv.org/

