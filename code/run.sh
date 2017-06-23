#!/bin/bash
#SBATCH -A SNIC2017-7-18
#SBATCH -o tests-%j.out
#SBATCH -p node
#SBATCH -t 00:02:00
#SBATCH -N 1
#SBATCH -n 20
#SBATCH -J N01-test_DTSW

#assert (ipn * nt  == 20 )
#assert ( p  * q   == P  )
#assert ( -N * ipn == P  ) 
#assert ( -N * 20  == -n ) 
#assert (  P * nt  == -n ) 

k=1;DLB=0
P=1;p=1;q=1;
ipn=$P;nt=20;
B=4;b=5;
iter=2

#assert ( B >= p ) 
#assert ( B >= q )



N=86111
#====================================

#module load intel intelmpi/17.4
module load gcc openmpi
JobID=${SLURM_JOB_ID}
app=./bin/dtsw_debug

params="-P $P -p $p -q $q -M $N $B $b -N $N $B $b -t $nt --ipn $ipn --iter-no $iter --timeout 100 --data-path ./data/tc5-86111-31-ep2.7-o4-gc-0.05/ "
echo "Params: $params"
tempdir=./temp
mkdir -p $tempdir
outfile=$tempdir/tests_${JobID}.out
echo "========================================================================================="
set -x 
rm $outfile
#srun  -n $P -c $nt -m cyclic:cyclic:* -l --output $outfile $app ${params}
#if [ "z$1z" == "zz" ]; then 
	mpirun  -n $P -output-filename $outfile   $app ${params}
#else
#	srun  -n $P $app ${params}
#fi
for i in $(seq 0 $[$P-1])
do
    grep "${i}:" $outfile >$tempdir/tests_p${i}.out
    str="s/$i://g"
    sed -i -e $str $tempdir/tests_p${i}.out
done 
grep "First task submitted" $tempdir/tests_p0.out > timing.txt
grep "Program finished" $tempdir/tests_p0.out >> timing.txt
grep "P:" $tempdir/tests_p0.out >> timing.txt
grep " timeout" ${outfile}* 
rm *file*.txt
