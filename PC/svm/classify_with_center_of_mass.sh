# Complete machine learning solution:

#
# Compute feature descriptor: center of mass position
#
cd ../center_of_mass_gather
make && ./center_of_mass_gather -i img_list_learning_mixed.txt --show -l 1320 -u 2760 -k 5 --label
# Learning dataset - You will be asked to label each image
# Write data in human readable form
cd ../svm/data
./json_prettify.sh dataset_training_com.json prettified_dataset_training_com.json
cd -

# Testing dataset - You will be asked to label each image
./center_of_mass_gather -i img_list_testing_mixed.txt --show -l 1320 -u 2760 -k 5 --label
# Write data in human readable form
cd ../svm/data
./json_prettify.sh dataset_training_com.json prettified_dataset_testing_com.json
cd -


#
# Train SVM using the computed descriptors
#
cd ../svm/svm_train_and_save
make && ./svm_train_and_save -f com -i ../data/prettified_dataset_training_com.json


#
# Test the SVM classifier
#
cd ../svm_test
make && ./svm_test -t ../svm_train_and_save/trained.svm -d ../data/prettified_dataset_testing_com.json -f com


