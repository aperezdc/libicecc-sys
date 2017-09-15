//
// build.rs
// Copyright (C) 2017 Adrian Perez <aperez@igalia.com>
// Distributed under terms of the MIT license.
//

extern crate gcc;
extern crate pkg_config;

fn main()
{
    let icecc = pkg_config::Config::new()
        .atleast_version("1.0.0")
        .probe("icecc")
        .unwrap();

    let mut build = gcc::Build::new();
    build.cpp(true).warnings(true);
    for path in icecc.include_paths {
         build.include(path);
    }

    build.file("iceccc.cc").compile("iceccc");
}
