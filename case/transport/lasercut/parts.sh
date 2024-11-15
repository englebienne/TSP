#!/usr/bin/zsh

echo "We're having $# parameters $@ "

if [[ $# -ge 1 ]]; then
    SCAD=$1
else
    SCAD="transportbox.scad"
fi

if [[ $# -ge 2 ]]; then
    NUM=$2
else
    NUM=12
fi


for ((I=0; I<${NUM};++I)); do
    echo -e "use <${SCAD}>\ncutmodel(${I});" > part${I}.scad
    openscad -o part${I}.svg part${I}.scad
    # sed -e 's/stroke="[^\"]*"/stroke="#ff0000"/g; s/ fill="[^\"]*"/ fill="none"/g; s/stroke-width=\"[^\"]*"/stroke-width="1pt"/g' < part${I}.svg > tmp.svg && mv tmp.svg part${I}.svg
    sed -e 's/stroke="[^\"]*"/stroke="#ff0000"/g; s/ fill="[^\"]*"/ fill="none"/g;' < part${I}.svg > tmp.svg && mv tmp.svg part${I}.svg
done
