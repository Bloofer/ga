FILES="${PWD}/proj1_instances/*"
for f in $FILES
do
  echo "Processing $f"
  filename=$(basename "$f")
  /home/yang/Sources/ga/proj1/maxcut $f > "1_ga_20_4_1p_random_noopt/result_$filename"
done
