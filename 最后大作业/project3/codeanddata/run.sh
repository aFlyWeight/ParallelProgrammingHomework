hdfs dfs -rm -f -r weather
hdfs dfs -rm -f -r weather-minmax
hdfs dfs -copyFromLocal data weather
hadoop jar hadoop-*streaming*.jar -file mapper.py -mapper mapper.py -file reducer.py -reducer reducer.py -input 'weather/*' -output weather-minmax
hdfs dfs -cat weather-minmax/part-00000
