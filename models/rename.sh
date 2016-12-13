for x in *; do
  case "$x" in
    *.*) ;;         # skip this file
    *) mv -- "$x" "$x.wobj";;
  esac
done
