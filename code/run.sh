#!/bin/bash
#SBATCH -A SNIC2017-7-18
#SBATCH -o tests-%j.out
#SBATCH -p devel
#SBATCH -t 00:35:00
#SBATCH -N 2
#SBATCH -n 40
#SBATCH -J N01-test_DTSW

#assert (ipn * nt  == 20 )
#assert ( p  * q   == P  )
#assert ( -N * ipn == P  ) 
#assert ( -N * 20  == -n ) 
#assert (  P * nt  == -n ) 

k=1;DLB=0
P=20;p=2;q=1
nt=1;
pure_mpi="--pure-mpi"
B=20;b=5

ipn=10;

iter=50

#assert ( B >= p ) 
#assert ( B >= q )



N=86111
#====================================

#module load gcc openmpi
module load intel intelmpi
JobID=${SLURM_JOB_ID}
app=./bin/dtsw_release
#data_path="/crex1/proj/snic2017-7-18/nobackup/ShallowWater/data/galew-42768-17-ep2.7-o4-gc-0.05/"
data_path="/crex1/proj/snic2017-7-18/nobackup/ShallowWater/data/galew-86111-31-ep2.7-o4-gc-0.05/"
params="-P $P -p $p -q $q -M $N $B $b -N $N $B $b -t $nt --ipn $ipn --iter-no $iter --timeout 2000 ${pure_mpi} --data-path ${data_path}" 
echo "Params: $params"
tempdir=./temp
mkdir -p $tempdir
outfile=$tempdir/tests_${JobID}.out
echo "========================================================================================="
set -x 
rm $outfile
if [ "z${CXX}z" == "zg++z" ] ; then 
	mpirun -n $P --map-by ppr:$ipn:node -output-filename $outfile   $app ${params}
else
	srun  --cpu_bind=cores --ntasks-per-node=$ipn --ntasks-per-core=1 -n $P -c $nt -m cyclic:cyclic:* -l --output $outfile $app ${params}
fi
for i in $(seq 0 $[$P-1])
do
    if [ $i < 10 ]; then
      i="0$i"
    fi
    grep "${i}:" $outfile >$tempdir/tests_${JobID}_p${i}.out
    str="s/$i://g"
    sed -i -e $str $tempdir/tests_${JobID}_p${i}.out
done 
echo "${JobID}:" >> timing.txt
grep "First task submitted" $tempdir/tests_${JobID}_p0.out >> timing.txt
grep "Program finished" $tempdir/tests_${JobID}_p0.out >> timing.txt
grep "P:" $tempdir/tests_${JobID}_p0.out >> timing.txt
grep " timeout" ${outfile}* 
grep "(\*\*\*" $tempdir/tests_${JobID}_p0.out > $tempdir/p0_${iter}.txt
grep "(\*\*\*" $tempdir/tests_${JobID}_p1.out > $tempdir/p1_${iter}.txt
rm *file*.txt
