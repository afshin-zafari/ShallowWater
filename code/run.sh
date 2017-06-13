#!/bin/bash
#SBATCH -A SNIC2017-7-18
#SBATCH -o tests-%j.out
#SBATCH -p node
#SBATCH -t 00:02:00
#SBATCH -N 2
#SBATCH -n 40
#SBATCH -J N02-StrongScaling_DT

#assert (ipn * nt  == 16 )
#assert ( p  * q   == P  )
#assert ( -N * ipn == P  ) 
#assert ( -N * 16  == -n ) 
#assert (  P * nt  == -n ) 

ipn=2;nt=16;
k=1;DLB=0
P=2;p=2;q=1;
B=2;b=2
N=6400
#====================================

#module load intel intelmpi/1.8
JobID=${SLURM_JOB_ID}
app=./bin/dtsw 

params="-P $P -p $p -q $q -M $N $B $b -N $N $B $b -t $nt --ipn $ipn --timeout 200"
echo "Params: $params"
tempdir=./temp
mkdir -p $tempdir
outfile=$tempdir/tests.out
echo "========================================================================================="
set -x 
rm $outfile
if [ "z$1z" == "zz" ]; then 
	mpirun  -ordered-output -n $P -ppn $ipn -outfile-pattern  $outfile -errfile-pattern $outfile  -l  $app ${params}
else
	mpirun  -ordered-output -n $P -ppn $ipn -l  $app ${params}
fi
grep "\[0\]" $outfile >$tempdir/tests_p0.out
grep "\[1\]" $outfile >$tempdir/tests_p1.out

sed -i -e 's/\[0\]//g' $tempdir/tests_p0.out
sed -i -e 's/\[1\]//g' $tempdir/tests_p1.out
 
rm *file*.txt
