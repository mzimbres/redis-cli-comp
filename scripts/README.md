
```sh
awk 'NR==1 { min=$2 } FNR==NR{min=($2+0<min)?$2:min;next} {print $1,$2/min}' foo.txt foo.txt
```

```sh
gh issue list --state all --json author --limit 10000 --jq '[ .[] | .author.login] | sort | unique | length'
```
