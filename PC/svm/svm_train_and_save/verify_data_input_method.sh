#!/usr/bin/env bash
# Check if using JSON input files is transparent for SVM learning
diff trained.svm trained.svm.orig
