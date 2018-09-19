double quick_select(double A[], int n, int k){
    int h, i, j;
    double tmp, pivot;

    pivot = A[0];
    for (h = i = j = 1; i < n; i++){
        if (A[i] > pivot){
            tmp = A[j];
            A[j] = A[i];
            A[i] = tmp;
            j++;
        } else if (A[i] == pivot) {
            A[i] = A[j];
            A[j] = A[h];
            h++;
            j++;
        }
    }

    if (j - h < k + 1 && k + 1 <= j) return pivot;
    else if (j < k+1) return quick_select(A+j, n-j, k-j);
    else return quick_select(A+h, j-h, k);
}
