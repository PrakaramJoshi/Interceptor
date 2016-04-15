#pragma once
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <time.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <atomic>
#include <functional>
#include <thread>
#include <filesystem>
#include "NonRecursiveLock.h"
#include "BlockingQueue.h"
#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
#include <Windows.h>
#endif
namespace AceLogger
{

	enum class MessageType{LOG_STATUS,LOG_ERROR,LOG_WARNING};

	struct Message{
		std::string p_msg;
		MessageType p_messageType;
		Message() {
			p_msg = "";
			p_messageType = MessageType::LOG_STATUS;
		};
		Message(const std::string &_msg, 
				const MessageType &_messageType) :
			p_msg(_msg), p_messageType(_messageType){
		};
		Message(std::string &&_msg,
			const MessageType &_messageType) :
			p_msg(std::move(_msg)), p_messageType(_messageType) {

		}
	};

	inline std::tm localtime(std::time_t const & time) {
		std::tm tm_snapshot;
#if (defined(__MINGW32__) || defined(__MINGW64__))
		memcpy(&tm_snapshot, ::localtime(&time), sizeof(std::tm));
#elif (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
		localtime_s(&tm_snapshot, &time);
#else
		localtime_r(&time, &tm_snapshot); // POSIX  
#endif
		return tm_snapshot;
	}

	// fwd declaration required for LogView class
	void ResetErrorCount();
	void ResetWarningCount();
	void ResetStatusCount();
	void static Log(std::string _msg, MessageType _type);
	/* LogView can be derived to push the log messages to custom viewer...*/
	class LogView {
		std::string m_username;
		std::string m_toolname;
		std::string m_starttime;
		std::string m_version;
		std::string m_platform;
		std::string m_path;
	public:
		LogView() {
			m_username = "";
			m_toolname = "";
			m_starttime = "";
			m_version = "";
			m_platform = "";
			m_path = "";
		}
		LogView(const std::string &_username,
			const std::string &_toolname,
			const std::string &_starttime,
			const std::string &_version,
			const std::string &_platform,
			const std::string &_path) : 
			m_username(_username),m_toolname(_toolname), 
			m_starttime(_starttime),m_version(_version), 
			m_platform(_platform),m_path(_path) {

		}

		virtual void init(const std::string &_username,
			const std::string &_toolname,
			const std::string &_starttime,
			const std::string &_version,
			const std::string &_platform,
			const std::string &_path) {
			m_username=_username;
			m_toolname=_toolname;
			m_starttime=_starttime;
			m_version=_version;
			m_platform = _platform;
			m_path = _path;
		}

		virtual void show(const std::string &_text) {
			std::cout << _text << std::endl;
		}

		virtual void reset_error_count() {
			AceLogger::ResetErrorCount();
		}

		virtual void reset_warning_count() {
			AceLogger::ResetWarningCount();
		}

		virtual void reset_status_count() {
			AceLogger::ResetStatusCount();
		}

		void add_log_mark(const std::string &_marker) {
			std::string marker_msg = "\n----------------------------------------------------------------------------------------------------\n" + _marker;
			AceLogger::Log(marker_msg, MessageType::LOG_STATUS);
		}

		void add_log_mark() {
			std::string marker_msg = "\n----------------------------------------------------------------------------------------------------";
			AceLogger::Log(marker_msg, MessageType::LOG_STATUS);
		}

		std::string get_username() {
			return m_username;
		}

		std::string get_platform() { 
			return m_platform;
		}

		std::string get_version() {
			return m_version;
		}

		std::string get_toolname() {
			return m_toolname;
		}

		std::string get_starttime() {
			return m_starttime;
		}

		std::string get_path() {
			return m_path;
		
		}

		virtual void finish_log() {

		}

		virtual void flush() {
			std::flush(std::cout);
		};
		virtual ~LogView() {
		};
	};
	class LogViewFile:public LogView {
		std::ofstream ofsLog;
	public:
		LogViewFile() {
		}

		virtual void init(const std::string &_user,
			const std::string &_toolName,
			const std::string &_starttime,
			const std::string &_version,
			const std::string &_platform,
			const std::string &_path) {

			LogView::init(
				_user,
				_toolName,
				_starttime,
				_version,
				_platform,
				_path);

			std::string dir = _path + "\\";
			dir += _toolName;
			
			if (!std::experimental::filesystem::exists(dir)) {
				if (!std::experimental::filesystem::create_directory(dir)) {
					std::runtime_error("unable to create logging dir " + dir);
				}
			}

			dir = dir + "\\";
			dir += _user;
			
			std::experimental::filesystem::path somepath(dir);
			if (!std::experimental::filesystem::exists(somepath)) {
				if (!std::experimental::filesystem::create_directory(dir)) {
					std::runtime_error("unable to create logging dir " + dir);
				}
			}

			dir += "\\";
			dir += _toolName+".log";
			ofsLog.open(dir.c_str(), std::fstream::in | std::fstream::out | std::fstream::app);
			ofsLog << "---------------------------------------------------------\n";
			ofsLog << "Header size \t\t\t= 6\n" ;
			ofsLog << "Log version	\t\t= 3.0\n" ;
			ofsLog << "Application name \t\t= " << _toolName<<"\n";
			ofsLog << "Application version \t\t= " << _version <<"\n";
			ofsLog << "platform	\t\t= " << _platform << "\n";
			ofsLog << "start time \t\t\t= " << _starttime << "\n";
			ofsLog << "---------------------------------------------------------\n";
		}
		virtual void show(const std::string &_text) {
			ofsLog << _text << std::endl;
		};

		virtual void flush() {
			ofsLog.flush();
		};

		virtual void finish_log() {
			flush();
			ofsLog.close();
		}

		virtual ~LogViewFile() {
			if(ofsLog.is_open())
				ofsLog.close();
		};
	};
	class Logger
	{
		LogViewFile				m_default_file_view;		

		std::string				m_toolname;
		std::string				m_username;
		std::string				m_version;
		std::string				m_starttime;
		std::string				m_log_dir;

		Interceptor::NonRecursiveLock			m_mutex;
		std::atomic<bool>		m_log_closed;

		std::atomic<int>		m_errorCount;
		std::atomic<int>		m_warningCount;
		std::atomic<int>		m_statusCount;

		std::atomic<size_t>		m_pending_logs;

		BlockingQueue<Message>	m_logMsgBuffer;
	public:
		
		
		std::thread *m_loggingThread;
		static Logger*& GetInstance(){
			static Logger *instance = new Logger();
			return instance;
		};

		static void DeInit() {
			bool did_log_thread_exit_unexpected = false;
#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
			DWORD result = WaitForSingleObject(GetInstance()->m_loggingThread->native_handle(), 0);

			if (result == WAIT_OBJECT_0) {
				// the thread handle is signaled - the thread has terminated
				did_log_thread_exit_unexpected = true;
			}
			else{
				// the thread handle is not signaled - the thread is still alive
			}
#endif
			
			if (did_log_thread_exit_unexpected) {
				// no point waiting for the log thread to finish
				// perform finish log in the current thread
				GetInstance()->finish_log_current_thread();
			}
			else {
				
				GetInstance()->finish_log();
				GetInstance()->m_loggingThread->join();
				delete GetInstance()->m_loggingThread;
			}
			
			GetInstance()->m_loggingThread = nullptr;
			delete GetInstance();
			GetInstance() = nullptr;
		}

		int get_error_count()const {
			return m_errorCount.load();
		}

		int get_warn_count()const {
			return m_warningCount.load();
		}

		int get_status_count()const {
			return m_statusCount.load();
		}

		void reset_error_count() {
			m_errorCount.store(0);
		}

		void reset_warn_count() {
			m_warningCount.store(0);
		}

		void reset_status_count() {
			m_statusCount.store(0);
		}

		void set_tool_name(const std::string &_toolname) {
			m_toolname = _toolname;
		}
		
		void set_version(const std::string &_version) {
			m_version = _version;
		}

		void set_log_dir(const std::string &_dir) {
			m_log_dir = _dir;
		}

		const std::string& get_toolname()const {
			return m_toolname;
		}

		std::string get_version()const {
			return m_version;
		}

		std::string get_username()const {
			return m_username;
		}

		std::string get_starttime()const {
			return m_starttime;
		}

		std::string get_log_dir()const {
			return m_log_dir;
		}

		size_t get_pending_logs()const {
			return m_pending_logs;
		}

		void add_log(Message *_msg) {
			m_pending_logs.fetch_add(1);
			m_logMsgBuffer.Insert(_msg);
		}

		void init() {
			
			try {
				m_loggingThread = new std::thread(std::bind(&AceLogger::Logger::LogMessage_Internal,
					this));
			}
		catch (const std::system_error& e) {
			std::cout << "Caught system_error with code " << e.code()
				<< " meaning " << e.what() << '\n';
		}
			

			std::string platform = "windows-x86";
#ifdef _WIN64
			platform = "windows-x64";
#endif 
#ifdef linux
			platform = "linux";
#endif
			m_default_file_view.init(get_username(),
				get_toolname(),
				get_starttime(),
				get_version(),
				platform,
				get_log_dir());

			m_pending_logs.store(0);
			
		}
		
		void inline log_flush_internal(){
			if (m_log_closed)
				return;
			while (m_logMsgBuffer.Size() > 0|| get_pending_logs()>0){
				std::this_thread::sleep_for(std::chrono::microseconds(10));
			}
			m_default_file_view.flush();
		}

		void finish_log() {
			Interceptor::UniqueGuard lock(m_mutex);
			if (m_log_closed)
				return;
			m_logMsgBuffer.ShutDown();

			log_flush_internal();

			m_default_file_view.finish_log();

			m_log_closed = true;
		}

		void finish_log_current_thread() {
			Interceptor::UniqueGuard lock(m_mutex);
			if (m_log_closed)
				return;
			m_log_closed = true;
			m_logMsgBuffer.ShutDown();
			LogMessage_Internal();
			m_default_file_view.finish_log();

		}

	private:
		~Logger(){
			
		};
		Logger(){
			m_log_closed = false;
			m_errorCount = 0;
			m_warningCount = 0;
			m_statusCount = 0;
			set_user_name();
			set_starttime();
			m_loggingThread = nullptr;
		}
		Logger(const Logger&);
		Logger& operator=(Logger const &);
		std::string  GetTimeString()
		{
			std::time_t t = std::time(NULL);
			std::tm timeinfo = AceLogger::localtime(t);
			char current_time[27];
			current_time[26] = '\0';
			sprintf_s(current_time, 27, "[%02d-%02d-%04d %02d:%02d:%02d] ", 
				timeinfo.tm_mon + 1, timeinfo.tm_mday, timeinfo.tm_year + 1900,
				timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
			return std::string(current_time);
		}

		void set_starttime() {
			m_starttime = GetTimeString();
		}
		void set_user_name() {
			m_username = "User";
			std::string env_variable = "USERNAME";
#ifdef linux
			env_variable = "USER";
#endif
#pragma warning(disable:4996)
			auto username = std::getenv(env_variable.c_str());
#pragma warning(default:4996)
			if (username) {
				m_username = std::string(username);
			}
		}

		void log_counts() {
			std::string timeStamp = GetTimeString();
			std::stringstream str;
			
			std::string message = timeStamp+" [STATUS]:\tTotal status messages: ";
			str << m_statusCount.load();
			message.append(str.str());
			m_default_file_view.show(message);

			str.str("");
			message = timeStamp + " [STATUS]:\tTotal warn messages: ";
			str << m_warningCount.load();
			message.append(str.str());
			m_default_file_view.show(message);

			str.str("");
			message = timeStamp + " [STATUS]:\tTotal error messages: ";
			str << m_errorCount.load();
			message.append(str.str());
			m_default_file_view.show(message);
		}

		void inline LogMessage_Internal() {
			Message *message = nullptr;
			while (m_logMsgBuffer.Remove(&message)) {
				if (!message) {
					m_pending_logs.fetch_sub(1);
					continue;
				}
				std::string timeStamp = GetTimeString();
				std::string msgType = "[STATUS]:\t";
				if ((message)->p_messageType == MessageType::LOG_STATUS) {
					m_statusCount++;
				}
				else if ((message)->p_messageType == MessageType::LOG_ERROR) {
					msgType = "[ERROR]:\t";
					m_errorCount++;

				}
				else if ((message)->p_messageType == MessageType::LOG_WARNING) {
					msgType = "[WARNING]:\t";
					m_warningCount++;
				}
				timeStamp.append(" ").append(msgType).append(message->p_msg);
				try {
					m_default_file_view.show(timeStamp);
				}
				catch (...) {
					throw std::runtime_error("logging system encountered runtime error");
				}
				delete message;
				m_pending_logs.fetch_sub(1);
			}
			log_counts();
			
		};
		
	};
	
	void static Log(std::string _msg,
					MessageType _type=MessageType::LOG_STATUS){
		auto msg = new Message(std::move(_msg), _type);
		Logger::GetInstance()->add_log(msg);
	};

	void static LogErr(const std::string &_msg) {

		Log(_msg, MessageType::LOG_ERROR);
	};

	void static LogWarn(const std::string &_msg) {

		Log(_msg, MessageType::LOG_WARNING);
	};

	void static log_flush(){
		Logger::GetInstance()->log_flush_internal();
	};

	void static Log_now(const std::string &_msg, 
						MessageType _type = MessageType::LOG_STATUS) {
		Log(_msg, _type);
		log_flush();
	}
	
	int static GetErrorCount(){
		return Logger::GetInstance()->get_error_count();
	}

	int static GetWarningCount(){
		return Logger::GetInstance()->get_warn_count();
	}

	int static GetStatusCount(){
		return Logger::GetInstance()->get_status_count();
	}

	static const std::string& GetToolName() {
		return Logger::GetInstance()->get_toolname();
	}

	void static ResetErrorCount() {
		Logger::GetInstance()->reset_error_count();
	}

	void static ResetStatusCount() {
		Logger::GetInstance()->reset_status_count();
	}

	void static ResetWarningCount() {
		Logger::GetInstance()->reset_warn_count();
	}

	void static ResetAllCounters(){
		Logger::GetInstance()->reset_error_count();
		Logger::GetInstance()->reset_status_count();
		Logger::GetInstance()->reset_warn_count();
	}

	void static FinishLog()
	{
		Logger::GetInstance()->finish_log();	
	}
	
	void static RegisterLogger(const std::string _path,
								const std::string _toolName,
								const std::string _toolVersion)
	{
		static std::atomic<bool> log_started = false;
		if (log_started.load()) {
			std::runtime_error("logging system already registered");
		}
		log_started = true;
		Logger::GetInstance()->set_tool_name(_toolName);
		Logger::GetInstance()->set_version(_toolVersion);
		Logger::GetInstance()->set_log_dir(_path);
		Logger::GetInstance()->init();
	};

	class register_logger_helper{
		
	public:
		~register_logger_helper() {
			AceLogger::Logger::GetInstance()->DeInit();
		};
		register_logger_helper(const std::string &_dir_path,
			const std::string &_proj_name,
			const std::string &_version){

			AceLogger::RegisterLogger(_dir_path, _proj_name, _version);
			AceLogger::Log("Starting Application...");
		};
	};
	
};

#define REGISTER_LOGGER(log_dir,tool_name,version_info) \
		AceLogger::register_logger_helper logger_helper(log_dir, tool_name, version_info);