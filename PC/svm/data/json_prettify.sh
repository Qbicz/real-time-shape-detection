DATASET_NAME=$1
echo $DATASET_NAME
echo "Prettify..."
cat $DATASET_NAME | python -m json.tool > "prettified_$DATASET_NAME"
