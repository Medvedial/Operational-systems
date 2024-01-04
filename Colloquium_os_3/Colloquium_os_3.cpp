#include <iostream>
#include <vector>
#include <random>
#include <thread>
#include <mutex>

class Matrix {
private:
    std::vector<std::vector<int>> data;
    int rows;
    int columns;

public:
    Matrix(int rows, int columns) : rows(rows), columns(columns) {
        data.resize(rows, std::vector<int>(columns, 0));
    }

    int getRows() const {
        return rows;
    }

    int getColumns() const {
        return columns;
    }

    int getElement(int i, int j) const {
        return data[i][j];
    }

    void setElement(int i, int j, int value) {
        data[i][j] = value;
    }
};

void fillMatrixRandomly(Matrix& matrix) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(1, 10);

    int rows = matrix.getRows();
    int columns = matrix.getColumns();

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < columns; ++j) {
            matrix.setElement(i, j, dist(gen));
        }
    }
}

void printMatrix(const Matrix& matrix) {
    int rows = matrix.getRows();
    int columns = matrix.getColumns();

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < columns; ++j) {
            std::cout << matrix.getElement(i, j) << " ";
        }
        std::cout << std::endl;
    }
}

void multiplyMatrices(const Matrix& matrixA, const Matrix& matrixB, Matrix& matrixC, int startRow, int endRow) {
    int n = matrixA.getColumns();
    int k = matrixB.getColumns();

    for (int i = startRow; i < endRow; ++i) {
        for (int j = 0; j < k; ++j) {
            int sum = 0;
            for (int l = 0; l < n; ++l) {
                sum += matrixA.getElement(i, l) * matrixB.getElement(l, j);
            }
            matrixC.setElement(i, j, sum);
        }
    }
}

int main() {
    setlocale(LC_ALL, "Rus");

    int m, n, k;
    std::cout << "Введите размеры матрицы A (m x n): ";
    std::cin >> m >> n;
    std::cout << "Введите размеры матрицы B (n x k): ";
    std::cin >> n >> k;

    Matrix matrixA(m, n);
    Matrix matrixB(n, k);
    Matrix matrixC(m, k);

    fillMatrixRandomly(matrixA);
    fillMatrixRandomly(matrixB);

    std::vector<std::thread> threads;
    int numThreads = std::thread::hardware_concurrency();

    int rowsPerThread = m / numThreads;
    int extraRows = m % numThreads;

    int startRow = 0;
    int endRow = 0;

    for (int i = 0; i < numThreads; ++i) {
        startRow = endRow;
        endRow = startRow + rowsPerThread;

        if (i < extraRows) {
            endRow++;
        }

        threads.emplace_back(multiplyMatrices, std::ref(matrixA), std::ref(matrixB), std::ref(matrixC), startRow, endRow);
    }

    for (auto& thread : threads) {
        thread.join();
    }

    std::cout << "Матрица A:" << std::endl;
    printMatrix(matrixA);

    std::cout << "Матрица B:" << std::endl;
    printMatrix(matrixB);

    std::cout << "Матрица C:" << std::endl;
    printMatrix(matrixC);

    return 0;
}

