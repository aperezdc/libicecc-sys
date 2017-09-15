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
    F(M_GET_INTERNALS, GetInternalsStatus, get_internals) \
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
