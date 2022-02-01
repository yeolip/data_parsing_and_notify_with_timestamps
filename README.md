# data_parsing_and_notify_with_timestamps
이 프로젝트는 ADTF(Automotive Data and Time-Triggered Framework) DAT로 <BR>
만들어진 Streaming데이터를 파싱하여 하여보고, <BR>
TimeStamp에 맞게 notify하도록 만들어 보았다.<BR>
추가적으로 정상적인 notify가 됬는지 영상을 출력하도록 지원하려한다.<BR>

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
이 프로젝트는 아파치 2.0 라이센스를 따르고 있다. 그리고 adtf streaming library(MPL-2.0)와 opencv(BSD-3-Clause)와 를 사용하고 있다.

This project is under the Apache 2.0 license. And adtf streaming library (MPL-2.0) and opencv (BSD-3-Clause) are used.

# Reference and code
https://www.elektrobit.com/products/automated-driving/eb-assist/adtf/
https://support.digitalwerk.net/adtf_libraries/adtf-streaming-library/v2/api/page_whats_new.html
https://opencv.org/

