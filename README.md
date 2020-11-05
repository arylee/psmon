# psmon
Process monitor. Currently only support linux system. If those process that had been configured was killed or exit normally, it will be relaunched.

Hello.jar and Hello2.jar are the same. It only print hello message using "System.out.println" then sleep 5 seconds in a dead loop. Please read Hello.java in the work directory. Parameters for spring cloud is useless here, just because there is a bug to using such parameters and now had been fixed.

# install
make && make clear

This program needs C++ 11 compiler. If you want to compile it with some old C++ compiler such as g++ 4.8.5, you have to change those code converting int to string using "std::to_string" to stringstream.

# run
PSMON_HOME enviroment must be set, or it will use the default /opt/psmon directory. For example, run it like this:

export PSMON_HOME=`pwd`

./psmon

./pscli status

./pscli stop hello

./pscli start hello

If you kill psomn with -9, you should use "./psmon -d" before restart it.
