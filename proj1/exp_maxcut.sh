FILES="${PWD}/proj1_instances/*"
for f in $FILES
do
  echo "Processing $f"
  filename=$(basename "$f")
  /home/yang/Sources/ga/proj1/maxcut $f > "7_ga_20_4_1p_rw_noopt/result1_$filename"
done

