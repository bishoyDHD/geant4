#!/bin/sh
#
# This file was produced by running the Configure script. It holds all the
# definitions figured out by Configure. Should you modify one of these values,
# do not forget to propagate your changes by running "Configure -der". You may
# instead choose to run each of the .SH files by yourself, or "Configure -S".
#

# Package name      : g4conf
# Source directory  : .
# Configuration time: Fri Jan 12 01:06:27 MSK 2001
# Configured by     : sadilov
# Target system     : osf1 ihep.su v4.0 564 alpha 

Author=''
Date=''
Header=''
Id=''
Locker=''
Log=''
Mcc=''
RCSfile=''
Revision=''
Source=''
State=''
ar=''
awk='/bin/awk'
bash=''
bison=''
byacc=''
c='\c'
cat='/bin/cat'
cf_by='sadilov'
cf_time='Fri Jan 12 01:06:27 MSK 2001'
chgrp=''
chmod=''
chown=''
comm=''
compress=''
contains='grep'
cp=''
cpio=''
cpp=''
csh=''
d_portable='undef'
date='/bin/date'
echo=''
egrep=''
emacs=''
eunicefix=':'
expr='/bin/expr'
find=''
flex=''
g4analysis_build='n'
g4analysis_build_jas='n'
g4analysis_build_lab='n'
g4analysis_build_lizard='n'
g4analysis_use='n'
g4analysis_use_jas='n'
g4analysis_use_lab='n'
g4analysis_use_lizard='n'
g4base='/usr/users/sadilov/geant4/source'
g4bin='/usr/users/sadilov/geant4/bin'
g4clhep_base_dir='/usr/clhep'
g4clhep_include_dir='/usr/local/include'
g4clhep_lib='CLHEP'
g4clhep_lib_dir='/usr/local/lib'
g4compiler='cxx'
g4conf='/usr/users/sadilov/work/g4conf/.config/bin/DEC-cxx'
g4data='/usr/users/sadilov/geant4/data'
g4debug=''
g4global='n'
g4granular='n'
g4include='/usr/users/sadilov/geant4/include'
g4install='/usr/users/sadilov/geant4'
g4ledata='/usr/users/sadilov/geant4/../G4EMLOW0.3'
g4levelgammadata='/usr/users/sadilov/geant4/data/PhotonEvaporation'
g4lib='/usr/users/sadilov/geant4/lib'
g4lib_build_shared='n'
g4lib_build_static='n'
g4lib_use_granular='n'
g4lib_use_shared='n'
g4lib_use_static='n'
g4neutronhpcrosssections='/usr/users/sadilov/geant4/../G4NDL0.2'
g4osname='DEC'
g4ospace_base_dir='/usr/users/sadilov/ObjectSpace'
g4radioactivedata='/usr/users/sadilov/geant4/data/RadiativeDecay'
g4system='DEC-cxx'
g4tmp='/usr/users/sadilov/geant4/tmp'
g4ui_build_gag_session='n'
g4ui_build_terminal_session='y'
g4ui_build_win32_session='n'
g4ui_build_xaw_session='n'
g4ui_build_xm_session='n'
g4ui_use_gag='n'
g4ui_use_tcsh='n'
g4ui_use_terminal='y'
g4ui_use_win32='n'
g4ui_use_xaw='n'
g4ui_use_xm='n'
g4use_ospace='n'
g4workdir='/usr/users/sadilov/geant4'
gcc=''
grep=''
gzip=''
hint='previous'
inews=''
ksh=''
less=''
line=''
lint=''
ln='/bin/ln'
lns='/bin/ln -s'
lp=''
lpr=''
ls=''
mail=''
mailx=''
make=''
mkdir=''
more=''
mv=''
myuname='osf1 ihep.su v4.0 564 alpha '
n=''
nroff=''
osname='dec_osf'
osvers='4.0'
package='g4conf'
perl=''
pg=''
pmake=''
pr=''
rm='/bin/rm'
rmail=''
sed='/bin/sed'
sendmail=''
sh='/bin/sh'
shar=''
sharpbang='#!'
shsharp='true'
sleep=''
smail=''
sort=''
spackage=''
spitshell='cat'
src='.'
startsh='#!/bin/sh'
submit=''
tail=''
tar=''
tbl=''
test='test'
touch='/bin/touch'
tr='/bin/tr'
troff=''
uname='/bin/uname'
uniq=''
uuname=''
vi=''
zcat=''
zip=''
CONFIG=true
# Variables propagated from previous config.sh file.
Author=''
CONFIG='true'
Date=''
Header=''
Id=''
Locker=''
Log=''
Mcc=''
RCSfile=''
Revision=''
Source=''
State=''
ar=''
awk='/bin/awk'
bash=''
bison=''
byacc=''
cat='/bin/cat'
cf_by='sadilov'
cf_time='Fri Jan 12 01:06:27 MSK 2001'
chgrp=''
chmod=''
chown=''
comm=''
compress=''
contains='grep'
cp=''
cpio=''
cpp=''
csh=''
d_portable='undef'
date='/bin/date'
echo=''
egrep=''
emacs=''
eunicefix=':'
expr='/bin/expr'
find=''
flex=''
g4analysis_build='n'
g4analysis_build_jas='n'
g4analysis_build_lab='n'
g4analysis_build_lizard='n'
g4analysis_use='n'
g4analysis_use_jas='n'
g4analysis_use_lab='n'
g4analysis_use_lizard='n'
g4base='/usr/users/sadilov/geant4/source'
g4bin='/usr/users/sadilov/geant4/bin'
g4clhep_base_dir='/usr/clhep'
g4clhep_include_dir='/usr/local/include'
g4clhep_lib='CLHEP'
g4clhep_lib_dir='/usr/local/lib'
g4compiler='cxx'
g4conf='/usr/users/sadilov/work/g4conf/.config/bin/DEC-cxx'
g4data='/usr/users/sadilov/geant4/data'
g4debug=''
g4global='n'
g4granular='n'
g4include='/usr/users/sadilov/geant4/include'
g4install='/usr/users/sadilov/geant4'
g4ledata='/usr/users/sadilov/geant4/../G4EMLOW0.3'
g4levelgammadata='/usr/users/sadilov/geant4/data/PhotonEvaporation'
g4lib='/usr/users/sadilov/geant4/lib'
g4lib_build_shared='n'
g4lib_build_static='n'
g4lib_use_granular='n'
g4lib_use_shared='n'
g4lib_use_static='n'
g4neutronhpcrosssections='/usr/users/sadilov/geant4/../G4NDL0.2'
g4osname='DEC'
g4ospace_base_dir='/usr/users/sadilov/ObjectSpace'
g4radioactivedata='/usr/users/sadilov/geant4/data/RadiativeDecay'
g4system='DEC-cxx'
g4tmp='/usr/users/sadilov/geant4/tmp'
g4ui_build_gag_session='n'
g4ui_build_terminal_session='y'
g4ui_build_win32_session='n'
g4ui_build_xaw_session='n'
g4ui_build_xm_session='n'
g4ui_use_gag='n'
g4ui_use_tcsh='n'
g4ui_use_terminal='y'
g4ui_use_win32='n'
g4ui_use_xaw='n'
g4ui_use_xm='n'
g4use_ospace='n'
g4workdir='/usr/users/sadilov/geant4'
gcc=''
grep=''
gzip=''
hint='previous'
inews=''
ksh=''
less=''
line=''
lint=''
ln='/bin/ln'
lns='/bin/ln -s'
lp=''
lpr=''
ls=''
mail=''
mailx=''
make=''
mkdir=''
more=''
mv=''
myuname='osf1 ihep.su v4.0 564 alpha '
n=''
nroff=''
osname='dec_osf'
osvers='4.0'
package='g4conf'
perl=''
pg=''
pmake=''
pr=''
rm='/bin/rm'
rmail=''
sed='/bin/sed'
sendmail=''
sh='/bin/sh'
shar=''
sharpbang='#!'
shsharp='true'
sleep=''
smail=''
sort=''
spackage=''
spitshell='cat'
src='.'
startsh='#!/bin/sh'
submit=''
tail=''
tar=''
tbl=''
test='test'
touch='/bin/touch'
tr='/bin/tr'
troff=''
uname='/bin/uname'
uniq=''
uuname=''
vi=''
zcat=''
zip=''
