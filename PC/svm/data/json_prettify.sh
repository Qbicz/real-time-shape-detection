DATASET_NAME=$1
echo $DATASET_NAME
cat $DATASET_NAME | python -m json.tool > $DATASET_NAME
