//
// lib.rs
// Copyright (C) 2017 Adrian Perez <aperez@igalia.com>
// Distributed under terms of the MIT license.
//

#![allow(dead_code, non_camel_case_types)]

extern crate libc;
use libc::{ c_char, c_int, c_uint, uint32_t, uint8_t, size_t };


#[repr(C)]
#[derive(Debug, Eq, PartialEq)]
pub enum CompileJobLanguage {
    C,
    CXX,
    OBJC,
    CUSTOM,
}


#[repr(C)]
#[derive(Debug, Eq, PartialEq)]
pub enum MsgType {
    M_UNKNOWN = 'A' as isize,
    M_PING,
    M_END,
    M_TIMEOUT, 
    M_GET_NATIVE_ENV,
    M_NATIVE_ENV,
    M_GET_CS,
    M_USE_CS,
    M_COMPILE_FILE,
    M_FILE_CHUNK,
    M_COMPILE_RESULT,
    M_JOB_BEGIN,
    M_JOB_DONE,
    M_JOB_LOCAL_BEGIN,
    M_JOB_LOCAL_DONE,
    M_LOGIN,
    M_STATS,
    M_MON_LOGIN,
    M_MON_GET_CS,
    M_MON_JOB_BEGIN,
    M_MON_JOB_DONE,
    M_MON_LOCAL_JOB_BEGIN,
    M_MON_STATS,
    M_TRANSFER_ENV,
    M_TEXT,
    M_STATUS_TEXT,
    M_GET_INTERNALS,
    M_CS_CONF,
    M_VERIFY_ENV,
    M_VERIFY_ENV_RESULT,
    M_BLACKLIST_HOST_ENV,
}


pub enum CompileJob {}
pub enum Msg {}
pub enum MsgChannel {}
pub enum DiscoverSched {}

macro_rules! msgmap {
    ($(($tag:ident => $type:ident))+) => {
        $(
            pub enum $type {}
            impl $type {
                const TYPE_TAG: MsgType = MsgType::$tag;
            }
        )+
    }
}


macro_rules! msg_as {
    ($t:ty, $e:expr) => {
        if msg_get_type($e) != $t::TYPE_TAG {
            panic!("invalid cast from {:?} to {}", msg_get_type($e), stringize!($t));
        }
        ($e as *mut $t)
    }
}


msgmap! {
    (M_PING => PingMsg)
    (M_END => EndMsg)
    (M_GET_CS => GetCSMsg)
    (M_USE_CS => UseCSMsg)
    (M_GET_NATIVE_ENV => GetNativeEnvMsg)
    (M_NATIVE_ENV => UseNativeEnvMsg)
    (M_COMPILE_FILE => CompileFileMsg)
    (M_FILE_CHUNK => FileChunkMsg)
    (M_COMPILE_RESULT => CompileResultMsg)
    (M_JOB_BEGIN => JobBeginMsg)
    (M_JOB_DONE => JobDoneMsg)
    (M_JOB_LOCAL_BEGIN => JobLocalBeginMsg)
    (M_JOB_LOCAL_DONE => JobLocalDoneMsg)
    (M_LOGIN => LoginMsg)
    (M_CS_CONF => ConfCSMsg)
    (M_STATS => StatsMsg)
    (M_TRANSFER_ENV => EnvTransferMsg)
    (M_GET_INTERNALS => GetInternalsStatus)
    (M_MON_LOGIN => MonLoginMsg)
    (M_MON_GET_CS => MonGetCSMsg)
    (M_MON_JOB_BEGIN => MonJobBeginMsg)
    (M_MON_JOB_DONE => MonJobDoneMsg)
    (M_MON_LOCAL_JOB_BEGIN => MonLocalJobBeginMsg)
    (M_MON_STATS => MonStatsMsg)
    (M_TEXT => TextMsg)
    (M_STATUS_TEXT => StatusTextMsg)
    (M_VERIFY_ENV => VerifyEnvMsg)
    (M_VERIFY_ENV_RESULT => VerifyEnvResultMsg)
    (M_BLACKLIST_HOST_ENV => BlacklistHostEnvMsg)
}


extern "C" {
    // CompileJob
    // TODO: Flags.
    pub fn compile_job_id(job: *const CompileJob) -> c_uint;
    pub fn compile_job_set_id(job: *mut CompileJob, j_id: c_uint);
    pub fn compile_job_language(job: *const CompileJob) -> CompileJobLanguage;
    pub fn compile_job_set_language(job: *mut CompileJob, lang: CompileJobLanguage);
    pub fn compile_job_compiler_name(job: *const CompileJob) -> *mut c_char;
    pub fn compile_job_set_compiler_name(job: *mut CompileJob, name: *const c_char);
    pub fn compile_job_environment_version(job: *const CompileJob) -> *mut c_char;
    pub fn compile_job_set_environment_version(job: *mut CompileJob, vers: *const c_char);
    pub fn compile_job_input_file(job: *const CompileJob) -> *mut c_char;
    pub fn compile_job_set_input_file(job: *mut CompileJob, file: *const c_char);
    pub fn compile_job_output_file(job: *const CompileJob) -> *mut c_char;
    pub fn compile_job_set_output_file(job: *mut CompileJob, file: *const c_char);
    pub fn compile_job_target_platform(job: *const CompileJob) -> *mut c_char;
    pub fn compile_job_set_target_platform(job: *mut CompileJob, plat: *const c_char);

    // Msg
    pub fn msg_free(msg: *mut Msg);
    pub fn msg_get_type(msg: *const Msg) -> MsgType;
    pub fn msg_fill_from_channel(msg: *mut Msg, chan: *mut MsgChannel);
    pub fn msg_send_to_channel(msg: *const Msg, chan: *mut MsgChannel);

    // PingMsg
    pub fn msg_ping_new() -> *mut PingMsg;

    // EndMsg
    pub fn msg_end_new() -> *mut EndMsg;

    // GetCSMsg
    pub fn msg_get_cs_new() -> *mut GetCSMsg;
    pub fn msg_get_cs_filename(msg: *const GetCSMsg) -> *mut c_char;
    pub fn msg_get_cs_set_filename(msg: *mut GetCSMsg, file: *const c_char);
    pub fn msg_get_cs_language(msg: *const GetCSMsg) -> CompileJobLanguage;
    pub fn msg_get_cs_set_language(msg: *mut GetCSMsg, lang: CompileJobLanguage);
    pub fn msg_get_cs_count(msg: *const GetCSMsg) -> uint32_t;
    pub fn msg_get_cs_set_count(msg: *mut GetCSMsg, count: uint32_t);
    pub fn msg_get_cs_target(msg: *const GetCSMsg) -> *mut c_char;
    pub fn msg_get_cs_set_target(msg: *mut GetCSMsg, target: *const c_char);
    pub fn msg_get_cs_client_id(msg: *const GetCSMsg) -> uint32_t;
    pub fn msg_get_cs_set_client_id(msg: *mut GetCSMsg, client_id: uint32_t);

    // UseCSMsg
    pub fn msg_use_cs_new() -> *mut UseCSMsg;
    pub fn msg_use_cs_job_id(msg: *const UseCSMsg) -> uint32_t;
    pub fn msg_use_cs_set_job_id(msg: *mut UseCSMsg, job_id: uint32_t);
    pub fn msg_use_cs_hostname(msg: *const UseCSMsg) -> *mut c_char;
    pub fn msg_use_cs_set_hostname(msg: *mut UseCSMsg, hostname: *const c_char);
    pub fn msg_use_cs_port(msg: *const UseCSMsg) -> uint32_t;
    pub fn msg_use_cs_set_port(msg: *mut UseCSMsg, port: uint32_t);
    pub fn msg_use_cs_host_platform(msg: *const UseCSMsg) -> *mut c_char;
    pub fn msg_use_cs_set_host_platform(msg: *mut UseCSMsg, platform: *const c_char);
    pub fn msg_use_cs_got_env(msg: *const UseCSMsg) -> uint32_t;
    pub fn msg_use_cs_set_got_env(msg: *mut UseCSMsg, got_env: uint32_t);
    pub fn msg_use_cs_client_id(msg: *const UseCSMsg) -> uint32_t;
    pub fn msg_use_cs_set_client_id(msg: *mut UseCSMsg, client_id: uint32_t);
    pub fn msg_use_cs_matched_job_id(msg: *const UseCSMsg) -> uint32_t;
    pub fn msg_use_cs_set_matched_job_id(msg: *mut UseCSMsg, matched_job_id: uint32_t);

    // GetNativeEnvMsg
    pub fn msg_get_native_env_new() -> *mut GetNativeEnvMsg;
    pub fn msg_get_native_env_compiler(msg: *const GetNativeEnvMsg) -> *mut c_char;
    pub fn msg_get_native_env_set_compiler(msg: *mut GetNativeEnvMsg, compiler: *const c_char);

    // UseNativeEnvMsg
    pub fn msg_use_native_env_new() -> *mut UseNativeEnvMsg;
    pub fn msg_use_native_env_version(msg: *const UseNativeEnvMsg) -> *mut c_char;
    pub fn msg_use_native_env_set_version(msg: *mut UseNativeEnvMsg, version: *const c_char);

    // CompileFileMsg
    pub fn msg_compile_file_new(job: *mut CompileJob, delete_job: bool) -> *mut CompileFileMsg;
    pub fn msg_compile_file_take_job(msg: *mut CompileFileMsg) -> *mut CompileJob;

    // FileChunkMsg
    pub fn msg_file_chunk_new(data: *mut uint8_t, len: size_t) -> *mut FileChunkMsg;

    // MsgChannel
    pub fn msg_channel_free(chan: *mut MsgChannel);
    pub fn msg_channel_set_bulk_transfer(chan: *mut MsgChannel);
    pub fn msg_channel_get_msg(chan: *mut MsgChannel) -> *mut Msg;
    pub fn msg_channel_get_msg_with_timeout(chan: *mut MsgChannel, timeout: c_int) -> *mut Msg;
    pub fn msg_channel_has_msg(chan: *const MsgChannel) -> bool;
    pub fn msg_channel_read_a_bit(chan: *mut MsgChannel);
    pub fn msg_channel_at_eof(chan: *const MsgChannel) -> bool;
    pub fn msg_channel_is_text_based(chan: *const MsgChannel) -> bool;

    // DiscoverSched
    pub fn discover_sched_new(name: *const c_char,) -> *mut DiscoverSched;
    pub fn discover_sched_new_with_options(netname: *const c_char,
                                           schedname: *const c_char,
                                           timeout: c_int) -> *mut DiscoverSched;
    pub fn discover_sched_free(dsched: *mut DiscoverSched);
    pub fn discover_sched_timed_out(dsched: *mut DiscoverSched) -> bool;
    pub fn discover_sched_listen_fd(dsched: *const DiscoverSched) -> c_int;
    pub fn discover_sched_connect_fd(dsched: *const DiscoverSched) -> c_int;
    pub fn discover_try_get_scheduler(dsched: *mut DiscoverSched) -> *mut MsgChannel;
    pub fn discover_sched_scheduler_name(dsched: *const DiscoverSched) -> *mut c_char;
}