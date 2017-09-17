/*
 * iceccc.cc
 * Copyright (C) 2017 Adrian Perez <aperez@igalia.com>
 *
 * Distributed under terms of the MIT license.
 */

#define C_DECL(decl) extern "C" decl __attribute__((nothrow))
#define C_WRAP(decl) C_DECL(decl); decl

#include <icecc/comm.h>
#include <icecc/job.h>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <string>

#define MESSAGE_KINDS(F) \
    F(M_PING, PingMsg, ping) \
    F(M_END, EndMsg, end) \
    F(M_GET_CS, GetCSMsg, get_cs) \
    F(M_USE_CS, UseCSMsg, use_cs) \
    F(M_GET_NATIVE_ENV, GetNativeEnvMsg, get_native_env) \
    F(M_NATIVE_ENV, UseNativeEnvMsg, use_native_env) \
    F(M_COMPILE_FILE, CompileFileMsg, compile_file) \
    F(M_FILE_CHUNK, FileChunkMsg, file_chunk) \
    F(M_COMPILE_RESULT, CompileResultMsg, compile_result) \
    F(M_JOB_BEGIN, JobBeginMsg, job_begin) \
    F(M_JOB_DONE, JobDoneMsg, job_done) \
    F(M_JOB_LOCAL_BEGIN, JobLocalBeginMsg, job_local_begin) \
    F(M_JOB_LOCAL_DONE, JobLocalDoneMsg, job_local_done) \
    F(M_LOGIN, LoginMsg, login) \
    F(M_CS_CONF, ConfCSMsg, conf_cs) \
    F(M_STATS, StatsMsg, stats) \
    F(M_TRANFER_ENV, EnvTransferMsg, env_transfer) \
    F(M_GET_INTERNALS, GetInternalStatus, get_internals) \
    F(M_MON_LOGIN, MonLoginMsg, mon_login) \
    F(M_MON_GET_CS, MonGetCSMsg, mon_get_cs) \
    F(M_MON_JOB_BEGIN, MonJobBeginMsg, mon_job_begin) \
    F(M_MON_JOB_DONE, MonJobDoneMsg, mon_job_done) \
    F(M_MON_LOCAL_JOB_BEGIN, MonLocalJobBeginMsg, mon_local_job_begin) \
    F(M_MON_STATS, MonStatsMsg, mon_stats) \
    F(M_TEXT, TextMsg, text) \
    F(M_STATUS_TEXT, StatusTextMsg, status_text) \
    F(M_VERIFY_ENV, VerifyEnvMsg, verify_env) \
    F(M_VERIFY_ENV_RESULT, VerifyEnvResultMsg, verify_env_result) \
    F(M_BLACKLIST_HOST_ENV, BlacklistHostEnvMsg, blacklist_host_env)

#define FWD_DECLARE_MSG_CTYPE(tag, type, name) \
    typedef struct C ## type C ## type;

MESSAGE_KINDS(FWD_DECLARE_MSG_CTYPE)

#undef FWD_DECLARE_MSG_CTYPE

#define C_WRAP_NEW(type, fname)                          \
    C_WRAP(C ## type* fname()) {                         \
        return reinterpret_cast<C ## type*>(new type()); \
    }


#define C_WRAP_OBJ_GET_STRING_ATTR(fname, type, attr)                       \
    C_WRAP(char* fname(const C ## type* obj)) {                             \
        assert(obj);                                                        \
        const auto& val = reinterpret_cast<const type*>(obj)->attr;         \
        char* c_val = reinterpret_cast<char*>(std::malloc(val.size() + 1)); \
        std::strncpy(c_val, val.data(), val.size());                        \
        return c_val;                                                       \
    }

#define C_WRAP_OBJ_GET_STRING(fname, type, accessor)                        \
    C_WRAP(char* fname(const C ## type* obj)) {                             \
        assert(obj);                                                        \
        std::string val = reinterpret_cast<const type*>(obj)->accessor();   \
        char* c_val = reinterpret_cast<char*>(std::malloc(val.size() + 1)); \
        std::strncpy(c_val, val.data(), val.size());                        \
        return c_val;                                                       \
    }

#define C_WRAP_OBJ_SET_STRING_ATTR(fname, type, attr)       \
    C_WRAP(void fname(C ## type* obj, const char* value)) { \
        assert(obj);                                        \
        assert(value);                                      \
        reinterpret_cast<type*>(obj)->attr = value;         \
    }

#define C_WRAP_OBJ_SET_STRING(fname, type, accessor)        \
    C_WRAP(void fname(C ## type* obj, const char* value)) { \
        assert(obj);                                        \
        assert(value);                                      \
        std::string s_value(value);                         \
        reinterpret_cast<type*>(obj)->accessor(s_value);    \
    }

#define C_WRAP_OBJ_GET_VALUE_ATTR(fname, type, ctype, attr) \
    C_WRAP(ctype fname(const C ## type* obj)) {             \
        assert(obj);                                        \
        return reinterpret_cast<ctype>(                     \
            reinterpret_cast<const type*>(obj)->attr);      \
    }

#define C_WRAP_OBJ_GET_VALUE(fname, type, ctype, accessor)           \
    C_WRAP(ctype fname(const C ## type* obj)) {                      \
        assert(obj);                                                 \
        auto value = reinterpret_cast<const type*>(obj)->accessor(); \
        return reinterpret_cast<ctype>(value);                       \
    }

#define C_WRAP_OBJ_SET_VALUE_ATTR(fname, type, ctype, cxxtype, attr) \
    C_WRAP(void fname(C ## type* obj, ctype value)) {                \
        assert(obj);                                                 \
        reinterpret_cast<type*>(obj)->attr =                         \
            reinterpret_cast<cxxtype>(value);                        \
    }

#define C_WRAP_OBJ_SET_VALUE(fname, type, ctype, cxxtype, accessor) \
    C_WRAP(void fname(C ## type* obj, ctype value)) {               \
        assert(obj);                                                \
        auto cxx_value = reinterpret_cast<cxxtype>(value);          \
        reinterpret_cast<type*>(obj)->accessor(cxx_value);          \
    }

#define C_WRAP_OBJ_STRING_ATTR_ACCESSORS(type, attr, fget, fset) \
    C_WRAP_OBJ_GET_STRING_ATTR(fget, type, attr) \
    C_WRAP_OBJ_SET_STRING_ATTR(fset, type, attr)

#define C_WRAP_OBJ_STRING_ACCESSORS(type, fget, aget, fset, aset) \
    C_WRAP_OBJ_GET_STRING(fget, type, aget) \
    C_WRAP_OBJ_SET_STRING(fset, type, aset)

#define C_WRAP_OBJ_VALUE_ATTR_ACCESSORS(type, attr, ctype, cxxtype, fget, fset) \
    C_WRAP_OBJ_GET_VALUE_ATTR(fget, type, ctype, attr) \
    C_WRAP_OBJ_SET_VALUE_ATTR(fset, type, ctype, cxxtype, attr)

#define C_WRAP_OBJ_VALUE_ACCESSORS(type, ctype, cxxtype, fget, aget, fset, aset) \
    C_WRAP_OBJ_GET_VALUE(fget, type, ctype, aget) \
    C_WRAP_OBJ_SET_VALUE(fset, type, ctype, cxxtype, aset)


typedef struct CCompileJob CCompileJob;
typedef struct CMsg CMsg;
typedef struct CMsgChannel CMsgChannel;
typedef struct CDiscoverSched CDiscoverSched;


//
// CompileJob
//
C_WRAP_OBJ_VALUE_ACCESSORS(CompileJob,
                           unsigned int, unsigned int,
                           compile_job_id, jobID,
                           compile_job_set_id, setJobID)

C_WRAP_OBJ_VALUE_ACCESSORS(CompileJob,
                           CompileJob::Language, CompileJob::Language,
                           compile_job_language, language,
                           compile_job_set_language, setLanguage)

C_WRAP_OBJ_STRING_ACCESSORS(CompileJob,
                            compile_job_compiler_name, compilerName,
                            compile_job_set_compiler_name, setCompilerName)

C_WRAP_OBJ_STRING_ACCESSORS(CompileJob,
                            compile_job_environment_version, environmentVersion,
                            compile_job_set_environment_version, setEnvironmentVersion)

C_WRAP_OBJ_STRING_ACCESSORS(CompileJob,
                            compile_job_input_file, inputFile,
                            compile_job_set_input_file, setInputFile)

C_WRAP_OBJ_STRING_ACCESSORS(CompileJob,
                            compile_job_output_file, outputFile,
                            compile_job_set_output_file, setOutputFile)


//
// Msg
//
C_WRAP(enum MsgType msg_get_type(const CMsg* msg))
{
    assert(msg);
    return reinterpret_cast<const Msg*>(msg)->type;
}

C_WRAP(void msg_free(CMsg* msg))
{
    assert(msg);
    delete reinterpret_cast<Msg*>(msg);
}

C_WRAP(void msg_fill_from_channel(CMsg* msg, CMsgChannel* chan))
{
    assert(msg);
    assert(chan);
    reinterpret_cast<Msg*>(msg)->fill_from_channel(reinterpret_cast<MsgChannel*>(chan));
}

C_WRAP(void msg_send_to_channel(const CMsg* msg, CMsgChannel* chan))
{
    assert(msg);
    assert(chan);
    reinterpret_cast<const Msg*>(msg)->send_to_channel(reinterpret_cast<MsgChannel*>(chan));
}

//
// PingMsg
//
C_WRAP_NEW(PingMsg, msg_ping_new)

//
// EndMsg
//
C_WRAP_NEW(EndMsg, msg_end_new)

//
// GetCSMsg
// TODO: versions, arg_flags, ignore_unverified
//
C_WRAP_NEW(GetCSMsg, msg_get_cs_new)

C_WRAP_OBJ_STRING_ATTR_ACCESSORS(GetCSMsg, filename,
                                 msg_get_cs_filename,
                                 msg_get_cs_set_filename)

C_WRAP_OBJ_VALUE_ATTR_ACCESSORS(GetCSMsg, lang,
                                CompileJob::Language, CompileJob::Language,
                                msg_get_cs_language,
                                msg_get_cs_set_language)

C_WRAP_OBJ_VALUE_ATTR_ACCESSORS(GetCSMsg, count,
                                uint32_t, uint32_t,
                                msg_get_cs_count,
                                msg_get_cs_set_count)

C_WRAP_OBJ_STRING_ATTR_ACCESSORS(GetCSMsg, target,
                                 msg_get_cs_target,
                                 msg_get_cs_set_target)

C_WRAP_OBJ_VALUE_ATTR_ACCESSORS(GetCSMsg, client_id,
                                uint32_t, uint32_t,
                                msg_get_cs_client_id,
                                msg_get_cs_set_client_id)

C_WRAP_OBJ_STRING_ATTR_ACCESSORS(GetCSMsg, preferred_host,
                                 msg_get_cs_preferref_host,
                                 msg_get_cs_set_preferred_host)

//
// UseCSMsg
//
C_WRAP_NEW(UseCSMsg, msg_use_cs_new)

C_WRAP_OBJ_VALUE_ATTR_ACCESSORS(UseCSMsg, job_id,
                                uint32_t, uint32_t,
                                msg_use_cs_job_id,
                                msg_use_cs_set_job_id)

C_WRAP_OBJ_STRING_ATTR_ACCESSORS(UseCSMsg, hostname,
                                 msg_use_cs_hostname,
                                 msg_use_cs_set_hostname)

C_WRAP_OBJ_VALUE_ATTR_ACCESSORS(UseCSMsg, port,
                                uint32_t, uint32_t,
                                msg_use_cs_port,
                                msg_use_cs_set_port)

C_WRAP_OBJ_STRING_ATTR_ACCESSORS(UseCSMsg, host_platform,
                                 msg_use_cs_host_platform,
                                 msg_use_cs_set_host_platform)

C_WRAP_OBJ_VALUE_ATTR_ACCESSORS(UseCSMsg, got_env,
                                uint32_t, uint32_t,
                                msg_use_cs_got_env,
                                msg_use_cs_set_got_env)

C_WRAP_OBJ_VALUE_ATTR_ACCESSORS(UseCSMsg, client_id,
                                uint32_t, uint32_t,
                                msg_use_cs_client_id,
                                msg_use_cs_set_client_id)

C_WRAP_OBJ_VALUE_ATTR_ACCESSORS(UseCSMsg, matched_job_id,
                                uint32_t, uint32_t,
                                msg_use_cs_matched_job_id,
                                msg_use_cs_set_matched_job_id)

//
// GetNativeEnvMsg
// TODO: extrafiles
//
C_WRAP_NEW(GetNativeEnvMsg, msg_get_native_env_new)

C_WRAP_OBJ_STRING_ATTR_ACCESSORS(GetNativeEnvMsg, compiler,
                                 msg_get_native_env_compiler,
                                 msg_get_native_env_set_compiler)

//
// UseNativeEnvMsg
//
C_WRAP_NEW(UseNativeEnvMsg, msg_use_native_env_new)

C_WRAP_OBJ_STRING_ATTR_ACCESSORS(UseNativeEnvMsg, nativeVersion,
                                 msg_use_native_env_version,
                                 msg_use_native_env_set_version)

//
// CompileFileMsg
//
C_WRAP(CCompileFileMsg* msg_compile_file_new(CCompileJob* job, bool delete_job))
{
    assert(job);
    auto result = new CompileFileMsg(reinterpret_cast<CompileJob*>(job), delete_job);
    return reinterpret_cast<CCompileFileMsg*>(result);
}

C_WRAP(CCompileJob* msg_compile_file_take_job(CCompileFileMsg* msg))
{
    assert(msg);
    auto result = reinterpret_cast<CompileFileMsg*>(msg)->takeJob();
    return reinterpret_cast<CCompileJob*>(result);
}

//
// JobDoneMsg
//
C_WRAP_NEW(JobDoneMsg, msg_job_done_new)

C_WRAP(CJobDoneMsg* msg_job_done_new_with_options(int job_id,
                                                  int exitcode,
                                                  unsigned int flags))
{
    return reinterpret_cast<CJobDoneMsg*>(new JobDoneMsg(job_id, exitcode, flags));
}

C_WRAP(void msg_job_done_set_from(CJobDoneMsg* msg, JobDoneMsg::from_type from))
{
    assert(msg);
    reinterpret_cast<JobDoneMsg*>(msg)->set_from(from);
}

C_WRAP(bool msg_job_done_is_from_server(CJobDoneMsg* msg))
{
    assert(msg);
    return reinterpret_cast<JobDoneMsg*>(msg)->is_from_server();
}

C_WRAP_OBJ_VALUE_ATTR_ACCESSORS(JobDoneMsg, real_msec,
                                uint32_t, uint32_t,
                                msg_job_done_real_msec,
                                msg_job_done_set_real_msec)

C_WRAP_OBJ_VALUE_ATTR_ACCESSORS(JobDoneMsg, user_msec,
                                uint32_t, uint32_t,
                                msg_job_done_user_msec,
                                msg_job_done_set_user_msec)

C_WRAP_OBJ_VALUE_ATTR_ACCESSORS(JobDoneMsg, sys_msec,
                                uint32_t, uint32_t,
                                msg_job_done_sys_msec,
                                msg_job_done_set_sys_msec)

C_WRAP_OBJ_VALUE_ATTR_ACCESSORS(JobDoneMsg, pfaults,
                                uint32_t, uint32_t,
                                msg_job_done_page_faults,
                                msg_job_done_set_page_faults)

C_WRAP_OBJ_VALUE_ATTR_ACCESSORS(JobDoneMsg, exitcode,
                                int, int,
                                msg_job_done_exit_code,
                                msg_job_done_set_exit_code)

C_WRAP_OBJ_VALUE_ATTR_ACCESSORS(JobDoneMsg, flags,
                                uint32_t, uint32_t,
                                msg_job_done_flags,
                                msg_job_done_set_flags)

C_WRAP_OBJ_VALUE_ATTR_ACCESSORS(JobDoneMsg, in_compressed,
                                uint32_t, uint32_t,
                                msg_job_done_compressed_bytes_in,
                                msg_job_done_set_compressed_bytes_in)

C_WRAP_OBJ_VALUE_ATTR_ACCESSORS(JobDoneMsg, in_uncompressed,
                                uint32_t, uint32_t,
                                msg_job_done_uncompressed_bytes_in,
                                msg_job_done_set_uncompressed_bytes_in)

C_WRAP_OBJ_VALUE_ATTR_ACCESSORS(JobDoneMsg, out_compressed,
                                uint32_t, uint32_t,
                                msg_job_done_compressed_bytes_out,
                                msg_job_done_set_compressed_bytes_out)

C_WRAP_OBJ_VALUE_ATTR_ACCESSORS(JobDoneMsg, out_uncompressed,
                                uint32_t, uint32_t,
                                msg_job_done_uncompressed_bytes_out,
                                msg_job_done_set_uncompressed_bytes_out)

//
// JobLocalBeginMsg
//
C_WRAP_NEW(JobLocalBeginMsg, msg_job_local_begin_new)

C_WRAP(CJobLocalBeginMsg* msg_job_local_begin_new_with_options(int job_id,
                                                               const char* file))
{
    assert(file);
    return reinterpret_cast<CJobLocalBeginMsg*>(new JobLocalBeginMsg(job_id, file));
}

C_WRAP_OBJ_STRING_ATTR_ACCESSORS(JobLocalBeginMsg, outfile,
                                 msg_job_local_begin_outfile,
                                 msg_job_local_begin_set_outfile)

C_WRAP_OBJ_VALUE_ATTR_ACCESSORS(JobLocalBeginMsg, stime,
                                uint32_t, uint32_t,
                                msg_job_local_begin_start_time,
                                msg_job_local_begin_set_start_time)

C_WRAP_OBJ_VALUE_ATTR_ACCESSORS(JobLocalBeginMsg, id,
                                uint32_t, uint32_t,
                                msg_job_local_begin_id,
                                msg_job_local_begin_set_id)

//
// JobLocalDoneMsg
//
C_WRAP_NEW(JobLocalDoneMsg, msg_job_local_done_new)

C_WRAP(CJobLocalDoneMsg* msg_job_local_done_new_with_id(unsigned int job_id))
{
    return reinterpret_cast<CJobLocalDoneMsg*>(new JobLocalDoneMsg(job_id));
}

C_WRAP_OBJ_VALUE_ATTR_ACCESSORS(JobLocalDoneMsg, job_id,
                                uint32_t, uint32_t,
                                msg_job_local_done_id,
                                msg_job_local_done_set_id)

//
// TODO: LoginMsg
//

//
// ConfCSMsg
//
C_WRAP_NEW(ConfCSMsg, msg_conf_cs_new)

C_WRAP(CConfCSMsg* msg_conf_cs_new_with_bench(const char* bench))
{
    assert(bench);
    return reinterpret_cast<CConfCSMsg*>(new ConfCSMsg(bench));
}

C_WRAP_OBJ_VALUE_ATTR_ACCESSORS(ConfCSMsg, max_scheduler_pong,
                                uint32_t, uint32_t,
                                msg_conf_cs_max_scheduler_pong,
                                msg_conf_cs_set_max_scheduler_pong)

C_WRAP_OBJ_VALUE_ATTR_ACCESSORS(ConfCSMsg, max_scheduler_ping,
                                uint32_t, uint32_t,
                                msg_conf_cs_max_scheduler_ping,
                                msg_conf_cs_set_max_scheduler_ping)

C_WRAP_OBJ_STRING_ATTR_ACCESSORS(ConfCSMsg, bench_source,
                                 msg_conf_cs_bench_source,
                                 msg_conf_cs_set_bench_source)

//
// StatsMsg
//
C_WRAP_NEW(StatsMsg, msg_stats_new)

C_WRAP_OBJ_VALUE_ATTR_ACCESSORS(StatsMsg, load,
                                uint32_t, uint32_t,
                                msg_stats_load,
                                msg_stats_set_load)

C_WRAP_OBJ_VALUE_ATTR_ACCESSORS(StatsMsg, loadAvg1,
                                uint32_t, uint32_t,
                                msg_stats_load_average_1min,
                                msg_stats_set_load_average_1min)

C_WRAP_OBJ_VALUE_ATTR_ACCESSORS(StatsMsg, loadAvg5,
                                uint32_t, uint32_t,
                                msg_stats_load_average_5min,
                                msg_stats_set_load_average_5min)

C_WRAP_OBJ_VALUE_ATTR_ACCESSORS(StatsMsg, loadAvg10,
                                uint32_t, uint32_t,
                                msg_stats_load_average_10min,
                                msg_stats_set_load_average_10min)

C_WRAP_OBJ_VALUE_ATTR_ACCESSORS(StatsMsg, freeMem,
                                uint32_t, uint32_t,
                                msg_stats_free_memory,
                                msg_stats_set_free_memory)

//
// EnvTransferMsg
//
C_WRAP_NEW(EnvTransferMsg, msg_env_transfer_new)

C_WRAP(CEnvTransferMsg* msg_env_transfer_new_with_options(const char* name,
                                                          const char* target))
{
    assert(name);
    assert(target);
    return reinterpret_cast<CEnvTransferMsg*>(new EnvTransferMsg(name, target));
}

C_WRAP_OBJ_STRING_ATTR_ACCESSORS(EnvTransferMsg, name,
                                 msg_env_transfer_name,
                                 msg_env_transfer_set_name)

C_WRAP_OBJ_STRING_ATTR_ACCESSORS(EnvTransferMsg, target,
                                 msg_env_transfer_target,
                                 msg_env_transfer_set_target)

//
// GetInternalStatus
//
C_WRAP_NEW(GetInternalStatus, msg_get_internal_status_new)

//
// MonLoginMsg
//
C_WRAP_NEW(MonLoginMsg, msg_mon_login_new)

//
// MonGetCSMsg
//
C_WRAP_NEW(MonGetCSMsg, msg_mon_get_cs_new)

C_WRAP(CMonGetCSMsg* msg_mon_get_cs_new_with_options(int job_id,
                                                     int client_id,
                                                     const CGetCSMsg* m))
{
    assert(m);
    auto msg = reinterpret_cast<GetCSMsg*>(const_cast<CGetCSMsg*>(m));
    return reinterpret_cast<CMonGetCSMsg*>(new MonGetCSMsg(job_id, client_id, msg));
}

C_WRAP_OBJ_VALUE_ATTR_ACCESSORS(MonGetCSMsg, job_id,
                                uint32_t, uint32_t,
                                msg_mon_get_cs_job_id,
                                msg_mon_get_cs_set_job_id)

C_WRAP_OBJ_VALUE_ATTR_ACCESSORS(MonGetCSMsg, clientid,
                                uint32_t, uint32_t,
                                msg_mon_get_cs_client_id,
                                msg_mon_get_cs_set_client_id)

//
// MonJobBeginMsg
//
C_WRAP_NEW(MonJobBeginMsg, msg_mon_job_begin_msg_new)

C_WRAP(CMonJobBeginMsg* msg_mon_job_begin_msg_new_with_options(unsigned int job_id,
                                                               unsigned int start_time,
                                                               unsigned int host_id))
{
    return reinterpret_cast<CMonJobBeginMsg*>(new MonJobBeginMsg(job_id, start_time, host_id));
}

C_WRAP_OBJ_VALUE_ATTR_ACCESSORS(MonJobBeginMsg, job_id,
                                uint32_t, uint32_t,
                                msg_mon_job_begin_msg_job_id,
                                msg_mon_job_begin_msg_set_job_id)

C_WRAP_OBJ_VALUE_ATTR_ACCESSORS(MonJobBeginMsg, stime,
                                uint32_t, uint32_t,
                                msg_mon_job_begin_msg_start_time,
                                msg_mon_job_begin_msg_set_start_time)

C_WRAP_OBJ_VALUE_ATTR_ACCESSORS(MonJobBeginMsg, hostid,
                                uint32_t, uint32_t,
                                msg_mon_job_begin_msg_host_id,
                                msg_mon_job_begin_msg_set_host_id)

//
// MonJobDoneMsg
//
C_WRAP_NEW(MonJobDoneMsg, msg_mon_job_done_new)

C_WRAP(CMonJobDoneMsg* msg_mon_job_done_new_with_job_done(const CJobDoneMsg* m))
{
    assert(m);
    auto msg = reinterpret_cast<const JobDoneMsg*>(m);
    return reinterpret_cast<CMonJobDoneMsg*>(new MonJobDoneMsg(*msg));
}

//
// MonLocalJobBeginMsg
//
C_WRAP_NEW(MonLocalJobBeginMsg, msg_mon_local_job_begin_new)

C_WRAP(CMonLocalJobBeginMsg* msg_mon_local_job_begin_new_with_options(unsigned int job_id,
                                                                      const char* file,
                                                                      unsigned int start_time,
                                                                      int host_id))
{
    assert(file);
    return reinterpret_cast<CMonLocalJobBeginMsg*>(new MonLocalJobBeginMsg(job_id, file, start_time, host_id));
}

C_WRAP_OBJ_VALUE_ATTR_ACCESSORS(MonLocalJobBeginMsg, job_id,
                                uint32_t, uint32_t,
                                msg_mon_local_job_begin_job_id,
                                msg_mon_local_job_begin_set_job_id)

C_WRAP_OBJ_VALUE_ATTR_ACCESSORS(MonLocalJobBeginMsg, stime,
                                uint32_t, uint32_t,
                                msg_mon_local_job_begin_start_time,
                                msg_mon_local_job_begin_set_start_time)

C_WRAP_OBJ_VALUE_ATTR_ACCESSORS(MonLocalJobBeginMsg, hostid,
                                uint32_t, uint32_t,
                                msg_mon_local_job_begin_host_id,
                                msg_mon_local_job_begin_set_host_id)

C_WRAP_OBJ_STRING_ATTR_ACCESSORS(MonLocalJobBeginMsg, file,
                                 msg_mon_local_job_begin_file,
                                 msg_mon_local_job_begin_set_file)

//
// MonStatsMsg
//
C_WRAP_NEW(MonStatsMsg, msg_mon_stats_new)

C_WRAP(CMonStatsMsg* msg_mon_stats_new_with_options(int host_id,
                                                    const char* message))
{
    assert(message);
    return reinterpret_cast<CMonStatsMsg*>(new MonStatsMsg(host_id, message));
}

C_WRAP_OBJ_VALUE_ATTR_ACCESSORS(MonStatsMsg, hostid,
                                uint32_t, uint32_t,
                                msg_mon_stats_host_id,
                                msg_mon_stats_set_host_id)

C_WRAP_OBJ_STRING_ATTR_ACCESSORS(MonStatsMsg, statmsg,
                                 msg_mon_stats_message,
                                 msg_mon_stats_set_message)

//
// TextMsg
//
C_WRAP_NEW(TextMsg, msg_text_new)

C_WRAP(CTextMsg* msg_text_new_with_text(const char* text))
{
    assert(text);
    return reinterpret_cast<CTextMsg*>(new TextMsg(text));
}

C_WRAP_OBJ_STRING_ATTR_ACCESSORS(TextMsg, text,
                                 msg_text_text,
                                 msg_text_set_text)

//
// StatusTextMsg
//
C_WRAP_NEW(StatusTextMsg, msg_status_text_new)

C_WRAP(CStatusTextMsg* msg_status_text_new_with_text(const char* text))
{
    assert(text);
    return reinterpret_cast<CStatusTextMsg*>(new StatusTextMsg(text));
}

C_WRAP_OBJ_STRING_ATTR_ACCESSORS(StatusTextMsg, text,
                                 msg_status_text_text,
                                 msg_status_text_set_text)

//
// FileChunkMsg
// TODO: Check whether accessors might be needed.
//
C_WRAP(CFileChunkMsg* msg_file_chunk_new(unsigned char* data, size_t len))
{
    assert(data);
    return reinterpret_cast<CFileChunkMsg*>(new FileChunkMsg(data, len));
}


//
// CompileResultMsg
//
C_WRAP_NEW(CompileResultMsg, msg_compile_result_new)

C_WRAP_OBJ_VALUE_ATTR_ACCESSORS(CompileResultMsg, status,
                                int, int,
                                msg_compile_result_status,
                                msg_compile_restul_set_status)

C_WRAP_OBJ_STRING_ATTR_ACCESSORS(CompileResultMsg, out,
                                 msg_compile_result_stdout,
                                 msg_compile_result_set_stdout)

C_WRAP_OBJ_STRING_ATTR_ACCESSORS(CompileResultMsg, err,
                                 msg_compile_result_stderr,
                                 msg_compile_result_set_stderr)

C_WRAP_OBJ_VALUE_ATTR_ACCESSORS(CompileResultMsg, was_out_of_memory,
                                bool, bool,
                                msg_compile_result_out_of_memory,
                                msg_compile_result_set_out_of_memory)

//
// JobBeginMsg
//
C_WRAP_NEW(JobBeginMsg, msg_job_begin_new)

C_WRAP(CJobBeginMsg* msg_job_begin_new_with_id(unsigned int job_id))
{
    return reinterpret_cast<CJobBeginMsg*>(new JobBeginMsg(job_id));
}

C_WRAP_OBJ_VALUE_ATTR_ACCESSORS(JobBeginMsg, stime,
                                uint32_t, uint32_t,
                                msg_job_begin_start_time,
                                msg_job_begin_set_tart_time)

//
// MsgChannel
//
C_WRAP(void msg_channel_free(CMsgChannel* chan))
{
    assert(chan);
    delete reinterpret_cast<MsgChannel*>(chan);
}

C_WRAP(void msg_channel_set_bulk_transfer(CMsgChannel* chan))
{
    assert(chan);
    reinterpret_cast<MsgChannel*>(chan)->setBulkTransfer();
}

C_WRAP(CMsg* msg_channel_get_msg(CMsgChannel* chan))
{
    assert(chan);
    auto result = reinterpret_cast<MsgChannel*>(chan)->get_msg();
    return reinterpret_cast<CMsg*>(result);
}

C_WRAP(bool msg_channel_has_msg(const CMsgChannel* chan))
{
    assert(chan);
    return reinterpret_cast<const MsgChannel*>(chan)->has_msg();
}

C_WRAP(CMsg* msg_channel_get_msg_with_timeout(CMsgChannel* chan, int timeout))
{
    assert(chan);
    auto result = reinterpret_cast<MsgChannel*>(chan)->get_msg(timeout);
    return reinterpret_cast<CMsg*>(result);
}

C_WRAP(void msg_channel_read_a_bit(CMsgChannel* chan))
{
    assert(chan);
    reinterpret_cast<MsgChannel*>(chan)->read_a_bit();
}

C_WRAP(bool msg_channel_at_eof(const CMsgChannel* chan))
{
    assert(chan);
    return reinterpret_cast<const MsgChannel*>(chan)->at_eof();
}

C_WRAP(bool msg_channel_is_text_based(const CMsgChannel* chan))
{
    assert(chan);
    return reinterpret_cast<const MsgChannel*>(chan)->is_text_based();
}


C_WRAP(CDiscoverSched* discover_sched_new(const char* name))
{
    if (name) {
        std::string s_name(name);
        return reinterpret_cast<CDiscoverSched*>(new DiscoverSched(s_name));
    } else {
        return reinterpret_cast<CDiscoverSched*>(new DiscoverSched());
    }
}

C_WRAP(CDiscoverSched* discover_sched_new_with_options(const char* netname,
                                                       const char* schedname,
                                                       int timeout))
{
    std::string s_netname = netname ? netname : std::string();
    std::string s_schedname = schedname ? schedname : std::string();
    return reinterpret_cast<CDiscoverSched*>(new DiscoverSched(s_netname, timeout, s_schedname));
}

C_WRAP(void discover_sched_free(CDiscoverSched* dsched))
{
    assert(dsched);
    delete reinterpret_cast<DiscoverSched*>(dsched);
}

C_WRAP(bool discover_sched_timed_out(CDiscoverSched* dsched))
{
    assert(dsched);
    return reinterpret_cast<DiscoverSched*>(dsched)->timed_out();
}

C_WRAP(int discover_sched_listen_fd(const CDiscoverSched* dsched))
{
    assert(dsched);
    return reinterpret_cast<const DiscoverSched*>(dsched)->listen_fd();
}

C_WRAP(int discover_sched_connect_fd(const CDiscoverSched* dsched))
{
    assert(dsched);
    return reinterpret_cast<const DiscoverSched*>(dsched)->connect_fd();
}

C_WRAP(CMsgChannel* discover_sched_try_get_scheduler(CDiscoverSched* dsched))
{
    assert(dsched);
    auto result = reinterpret_cast<DiscoverSched*>(dsched)->try_get_scheduler();
    return reinterpret_cast<CMsgChannel*>(result);
}

C_WRAP(char* discover_sched_scheduler_name(const CDiscoverSched* dsched))
{
    assert(dsched);
    auto name = reinterpret_cast<const DiscoverSched*>(dsched)->schedulerName();
    char* c_name = reinterpret_cast<char*>(std::malloc(name.size() + 1));
    std::strncpy(c_name, name.data(), name.size());
    return c_name;
}

C_WRAP(char* discover_sched_network_name(const CDiscoverSched* dsched))
{
    assert(dsched);
    auto name = reinterpret_cast<const DiscoverSched*>(dsched)->networkName();
    char* c_name = reinterpret_cast<char*>(std::malloc(name.size() + 1));
    std::strncpy(c_name, name.data(), name.size());
    return c_name;
}
