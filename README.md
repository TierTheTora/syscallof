# syscallof
## usage
```
syscallof [OPTIONS...] IDENTIFIERS...
Options:
	-x, --x86_64   use x86_64  syscall table
	-a, --arm      use arm     syscall table
	-A, --aarch64  use aarch64 syscall table
	-X, --x86      use x86     syscall table
	
	-h, --help  show the help menu
	-n, --name  search by syscall name
```
## compile
```bash
git clone https://github.com/TierTheTora/syscallof.git
cd syscallof  # or which ever directory it was cloned to
make
```
