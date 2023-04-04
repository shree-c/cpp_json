if [[ -e $1 ]] then
  if [[ -d $2 ]] then
    "./$1" $2
  else
    echo "$2 doesn't exist"
  fi
else
  echo "$1 doesn't exist"
fi
