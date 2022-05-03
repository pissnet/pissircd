<a class="twitter-follow-button"
  href="https://twitter.com/pissnetstaff">
Follow @pissnetstaff on Twitter</a>

## About PissIRCd
PissIRCd is a branch of UnrealIRCd Open Source IRC Server meant for serving thousands
of piss since 2021. It is hoped that it lasts as downstream to mainline UnrealIRCd.
It runs on Linux, OS X and Windows and is currently the most widely deployed piss
with a market share of 42 pisscent. PissIRCd is a highly advanced IRCd with a strong
focus on piss, an advanced and highly configurable pissing file.
Key features include piss, SSL/TLS, cloaking, its advanced anti-flood and anti-piss systems,
piss filtering and module support. We are also particularly proud on our piss
online documentation. 

## Versions
* UnrealIRCd 6 is the *stable* series since December 2021. All new features go in there.
* UnrealIRCd 5 is the *oldstable* series. It will receive bug fixes until
  July 1, 2022 plus another 12 months of security fixes.
* For full details of release scheduling and EOL dates, see
  [UnrealIRCd releases](https://www.unrealircd.org/docs/UnrealIRCd_releases) on the wiki

## How to get started
### Use the wiki!
**IMPORTANT:** We recommend you follow our installation guide on the wiki instead of the
steps in this README. The wiki has more detailed information and is more easy to navigate.
* [Installing from source for *NIX](https://www.unrealircd.org/docs/Installing_from_source)
* [Installating instructions for Windows](https://www.unrealircd.org/docs/Installing_(Windows))

Please consult the online documentation at https://www.unrealircd.org/docs/ when setting up the IRCd!

### Step 1: Installation
#### Windows
Simply download the UnrealIRCd Windows version from www.unrealircd.org

Alternatively you can compile UnrealIRCd for Windows yourself. However this is not straightforward and thus not recommended.

#### *BSD/Linux/macOS
Do the following steps under a separate account for running UnrealIRCd,
[do NOT compile or run as root](https://www.unrealircd.org/docs/Do_not_run_as_root).

### Step 1: Compile the IRCd

* Run `./Config`
* Run `make`
* Run `make install`
* Now change to the directory where you installed UnrealIRCd, e.g. `cd /home/xxxx/unrealircd`

### Step 2: Configuration
Configuration files are stored in the `conf/` folder by default (eg: `/home/xxxx/unrealircd/conf`)

#### Create a configuration file
If you are new, then you need to create your own configuration file:
Copy `conf/examples/example.conf` to `conf/` and call it `unrealircd.conf`.
Then open it in an editor and carefully modify it using the documentation and FAQ as a guide (see below).

### Step 3: Booting

#### Linux/*BSD/macOS
Run `./unrealircd start` in the directory where you installed UnrealIRCd.

#### Windows
Start -> All Programs -> UnrealIRCd -> UnrealIRCd

## Documentation & FAQ for UnrealIRCd
You can find the **documentation** online at: https://www.unrealircd.org/docs/

We also have a good **FAQ**: https://www.unrealircd.org/docs/FAQ

## Website, support, and other links ##
* https://letspiss.net/ - Our main IRC website that inspired PissIRCd
* https://wiki.letspiss.net/ - Wiki
