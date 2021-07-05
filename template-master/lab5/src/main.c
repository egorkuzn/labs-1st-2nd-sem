#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <limits.h>
#include <string.h>

#define MAX_CHAR_NUM 256
#define ZIP_TEXT 'c'
#define UNZIP_TEXT 'd'
#define BUFFER_SIZE 20
#define BYTE_LENGTH 8
#define FILE_EMPTY -1

typedef unsigned char ui8;

typedef struct readWriteFileBits {
    FILE* out;
    FILE* in;
    ui8 bufferData[BUFFER_SIZE];
    size_t bufferIdx; // <= BUFFER_SIZE
    ui8 bitsContainer[BYTE_LENGTH];
    size_t bitsIdx;
} TRWFileBits;

typedef struct huffmanTree {
    struct huffmanTree* left;
    struct huffmanTree* right;
    ui8 character;
    int encodingTreeFrequency;
} THuffmanTree;

typedef struct binaryCodesTable {
    ui8* bits;
    ui8 character;
    ui8 bitsLen; // <= 8
} TCodesTable;

bool FileDoesntEnd(const size_t bufferIdx, const size_t currentBufferSize) {
    return bufferIdx != currentBufferSize || currentBufferSize == BUFFER_SIZE;
}

bool IsBufferOverflow(const TRWFileBits* file_manager) {
    return file_manager->bufferIdx == BUFFER_SIZE;
}

void UpdateBuffer(size_t* inputSize, TRWFileBits* file_manager) {
    *inputSize = fread(file_manager->bufferData, sizeof(ui8), BUFFER_SIZE, file_manager->in);
    file_manager->bufferIdx = 0;
}

int GetElementByItsBinaryCode(const ui8* binCodeSequence) {
    int element = 0;
    for (int i = 0; i < BYTE_LENGTH; i++) {
        element = (element << 1) | binCodeSequence[i];
    }
    return element;
}

ui8 GetElementsBit(const ui8 element, const int bitIdx) {
    return (bool)((1 << bitIdx) & element);
}

int QElemCompare(const void* a, const void* b) {
    return (((THuffmanTree*)a)->encodingTreeFrequency) - (((THuffmanTree*)b)->encodingTreeFrequency);
}

void InitHuffmanPriorityQueue(const size_t initialSize, THuffmanTree* queue) {
    for (size_t i = 0; i < initialSize; i++) {
        queue[i].character = (ui8)i;
        queue[i].encodingTreeFrequency = INT_MAX;
        queue[i].left = NULL;
        queue[i].right = NULL;
    }
}

bool IsLeafYetNotVisited(const THuffmanTree treeLeaf) {
    return treeLeaf.encodingTreeFrequency == INT_MAX;
}

void FillLeafsDataToPriorityQueue(size_t* leavesCount, TRWFileBits* file_manager, THuffmanTree* queue) {
    ui8 element = file_manager->bufferData[file_manager->bufferIdx];
    file_manager->bufferIdx += 1;
    if (IsLeafYetNotVisited(queue[element])) {
        queue[element].encodingTreeFrequency = 0;
        *leavesCount += 1;
    }
    queue[element].encodingTreeFrequency++;
}

THuffmanTree* InputTextToPriorityQueue(TRWFileBits* file_manager, size_t* realSize) {
    THuffmanTree* queue = (THuffmanTree*)calloc(MAX_CHAR_NUM, sizeof(THuffmanTree));
    InitHuffmanPriorityQueue(MAX_CHAR_NUM, queue);
    size_t inputSize = fread(file_manager->bufferData, sizeof(ui8), BUFFER_SIZE, file_manager->in);
    if (inputSize == 0) {
        *realSize = 0;
        free(queue);
        return NULL;
    }
    file_manager->bufferIdx = 0;
    FillLeafsDataToPriorityQueue(realSize, file_manager, queue);
    while (FileDoesntEnd(file_manager->bufferIdx, inputSize)) {
        FillLeafsDataToPriorityQueue(realSize, file_manager, queue);
        if (IsBufferOverflow(file_manager)) {
            UpdateBuffer(&inputSize, file_manager);
        }
    }
    qsort(queue, MAX_CHAR_NUM, sizeof(THuffmanTree), QElemCompare);
    fclose(file_manager->in);
    return queue;
}

void SwapQElem(THuffmanTree* first, THuffmanTree* second) {
    THuffmanTree copy = *first;
    *first = *second;
    *second = copy;
}

void RaiseElementToTheTopInBinHeap(size_t currentIndex, THuffmanTree* priorityQueue) {
    while ((currentIndex + 1) / 2 != 0 && priorityQueue[currentIndex].encodingTreeFrequency < priorityQueue[(currentIndex - 1) / 2].encodingTreeFrequency) {
        SwapQElem(&priorityQueue[currentIndex], &priorityQueue[(currentIndex - 1) / 2]);
        currentIndex = (currentIndex - 1) / 2;
    }
}

bool IsNotThereNextDataToHeapify(const int idxQueue) {
    return idxQueue <= 0;
}

void CreateCodesTable(int dynamicCodesIdx, THuffmanTree* root, size_t* tableIdx,
    TCodesTable* codesTable, ui8* dynamicSymbolsBinaryCode) {
    if (root->left != NULL) {
        dynamicCodesIdx++;
        dynamicSymbolsBinaryCode[dynamicCodesIdx - 1] = 0;
        CreateCodesTable(dynamicCodesIdx, root->left, tableIdx, codesTable,
            dynamicSymbolsBinaryCode);
        dynamicCodesIdx--;
    }
    if (root->right != NULL) {
        dynamicCodesIdx++;
        dynamicSymbolsBinaryCode[dynamicCodesIdx - 1] = 1;
        CreateCodesTable(dynamicCodesIdx, root->right, tableIdx, codesTable,
            dynamicSymbolsBinaryCode);
        dynamicCodesIdx--;
    }
    if (root->left == NULL && root->right == NULL) {
        if (dynamicCodesIdx == 0) {
            dynamicSymbolsBinaryCode[0] = 0;
            dynamicCodesIdx++;
        }
        codesTable[*tableIdx].character = root->character;
        codesTable[*tableIdx].bits = (ui8*)calloc(dynamicCodesIdx, sizeof(ui8));
        memcpy(codesTable[*tableIdx].bits, dynamicSymbolsBinaryCode, sizeof(ui8) * dynamicCodesIdx);
        codesTable[*tableIdx].bitsLen = dynamicCodesIdx;
        *tableIdx += 1;
    }
}

THuffmanTree* CopyHuffmanTree(const THuffmanTree copyFrom) {
    THuffmanTree* tree = (THuffmanTree*)calloc(1, sizeof(THuffmanTree));
    tree->character = copyFrom.character;
    tree->right = copyFrom.right;
    tree->left = copyFrom.left;
    tree->encodingTreeFrequency = copyFrom.encodingTreeFrequency;
    return tree;
}

void HeapifyBinPriorityQueue(const int idxQueue, THuffmanTree* priorityQueue) {
    if (IsNotThereNextDataToHeapify(idxQueue)) {
        return;
    }
    int movementIdx = 0;
    int firstChildIdx = 1;
    int secondChildIdx = 2;
    while (firstChildIdx <= idxQueue &&
        (priorityQueue[movementIdx].encodingTreeFrequency > priorityQueue[firstChildIdx].encodingTreeFrequency ||
            priorityQueue[movementIdx].encodingTreeFrequency > priorityQueue[secondChildIdx].encodingTreeFrequency)) {
        if (secondChildIdx == idxQueue + 1) {
            if (priorityQueue[movementIdx].encodingTreeFrequency > priorityQueue[firstChildIdx].encodingTreeFrequency) {
                SwapQElem(&priorityQueue[movementIdx], &priorityQueue[firstChildIdx]);
            }
            movementIdx = firstChildIdx;
        }
        else if (priorityQueue[firstChildIdx].encodingTreeFrequency <= priorityQueue[secondChildIdx].encodingTreeFrequency) {
            SwapQElem(&priorityQueue[movementIdx], &priorityQueue[firstChildIdx]);
            movementIdx = firstChildIdx;
        }
        else {
            SwapQElem(&priorityQueue[movementIdx], &priorityQueue[secondChildIdx]);
            movementIdx = secondChildIdx;
        }
        firstChildIdx = movementIdx * 2 + 1;
        secondChildIdx = movementIdx * 2 + 2;
    }
}

THuffmanTree GetTopFromPriorityQueue(THuffmanTree* priorityQueue, int* idxQueue) {
    THuffmanTree saveTop = priorityQueue[0];
    SwapQElem(&priorityQueue[0], &priorityQueue[*idxQueue]);
    *idxQueue -= 1;
    HeapifyBinPriorityQueue(*idxQueue, priorityQueue);
    return saveTop;
}

void PutTreeToPriorityQueue(int* idxQueue, THuffmanTree* unionLeaves, THuffmanTree* priorityQueue) {
    *idxQueue += 1;
    SwapQElem(&priorityQueue[*idxQueue], unionLeaves);
    free(unionLeaves);
    RaiseElementToTheTopInBinHeap(*idxQueue, priorityQueue);
}

THuffmanTree* BuildHuffmanCodingTree(const size_t realSize, THuffmanTree* priorityQueue) {
    int idxQueue = (int)(realSize - 1);
    if (idxQueue == 0) {
        priorityQueue[idxQueue].encodingTreeFrequency = 0;
    }
    while (idxQueue != 0) {
        THuffmanTree testOne = GetTopFromPriorityQueue(priorityQueue, &idxQueue);
        THuffmanTree testTwo = GetTopFromPriorityQueue(priorityQueue, &idxQueue);
        THuffmanTree* unionLeaves = (THuffmanTree*)calloc(1, sizeof(THuffmanTree));
        unionLeaves->encodingTreeFrequency = testOne.encodingTreeFrequency + testTwo.encodingTreeFrequency;
        THuffmanTree* leftChild = CopyHuffmanTree(testOne);
        THuffmanTree* rightChild = CopyHuffmanTree(testTwo);
        unionLeaves->left = leftChild;
        unionLeaves->right = rightChild;
        PutTreeToPriorityQueue(&idxQueue, unionLeaves, priorityQueue);
    }
    return CopyHuffmanTree(priorityQueue[0]);
}

void CopyElementToBitsContainer(const ui8 element, TRWFileBits* file_manager) {
    file_manager->bitsContainer[file_manager->bitsIdx] = element;
    file_manager->bitsIdx++;
    if (file_manager->bitsIdx == BYTE_LENGTH) {
        ui8 elementToWrite = (ui8)GetElementByItsBinaryCode(file_manager->bitsContainer);
        fwrite(&elementToWrite, sizeof(ui8), 1, file_manager->out);
        file_manager->bitsIdx = 0;
    }
}

void WriteHuffmanTreeToFile(THuffmanTree* root, TRWFileBits* file_manager) {
    if (root->left != NULL) {
        CopyElementToBitsContainer(1, file_manager);
        WriteHuffmanTreeToFile(root->left, file_manager);
    }
    if (root->right != NULL) {
        WriteHuffmanTreeToFile(root->right, file_manager);
    }
    if (root->left == NULL && root->right == NULL) {
        CopyElementToBitsContainer(0, file_manager);
        for (int i = 0; i < BYTE_LENGTH; i++) {
            ui8 currentBit = GetElementsBit(root->character, BYTE_LENGTH - i - 1);
            CopyElementToBitsContainer(currentBit, file_manager);
        }
    }
}

bool IsNeededToWriteMissingBits(const size_t bitsIdx) {
    return bitsIdx != 0 && bitsIdx <= BYTE_LENGTH;
}

void WriteMissingBits(TRWFileBits* file_manager, ui8* neededBits) {
    for (int i = file_manager->bitsIdx; i < BYTE_LENGTH; i++) {
        file_manager->bitsContainer[i] = 0;
        *neededBits += 1;
    }
    ui8 elementToWrite = (ui8)GetElementByItsBinaryCode(file_manager->bitsContainer);
    fwrite(&elementToWrite, sizeof(ui8), 1, file_manager->out);
}

void EncodeTextWithNewCharacters(const size_t realSize, TCodesTable* codesTable,
    TRWFileBits* file_manager, ui8* neededBits) {
    fseek(file_manager->in, 3, SEEK_CUR);
    size_t inputSize = fread(file_manager->bufferData, sizeof(ui8), BUFFER_SIZE, file_manager->in);
    file_manager->bufferIdx = 0;
    while (FileDoesntEnd(file_manager->bufferIdx, inputSize)) {
        for (size_t tableIdx = 0; tableIdx < realSize; tableIdx++) {
            if (codesTable[tableIdx].character == file_manager->bufferData[file_manager->bufferIdx]) {
                for (int i = 0; i < codesTable[tableIdx].bitsLen; i++) {
                    CopyElementToBitsContainer(codesTable[tableIdx].bits[i], file_manager);
                }
                file_manager->bufferIdx++;
                break;
            }
        }
        if (IsBufferOverflow(file_manager)) {
            UpdateBuffer(&inputSize, file_manager);
        }
    }
    if (IsNeededToWriteMissingBits(file_manager->bitsIdx)) {
        WriteMissingBits(file_manager, neededBits);
    }
}

void WriteNewCodesToFile(const size_t realSize, THuffmanTree* huffmanTree,
    TCodesTable* codesTable,
    TRWFileBits* file_manager, ui8* neededBits) {
    file_manager->in = fopen("in.txt", "rb");
    file_manager->bitsIdx = 0;
    WriteHuffmanTreeToFile(huffmanTree, file_manager);
    EncodeTextWithNewCharacters(realSize, codesTable, file_manager, neededBits);
}

bool IsBitsContainerOverflow(const TRWFileBits* file_manager) {
    return file_manager->bitsIdx == BYTE_LENGTH;
}

void UpdateBitsContainer(TRWFileBits* file_manager, size_t* inputSize) {
    file_manager->bitsIdx = 0;
    file_manager->bufferIdx++;
    if (IsBufferOverflow(file_manager)) {
        UpdateBuffer(inputSize, file_manager);
    }
    for (int copyIdx = 0; copyIdx < BYTE_LENGTH; copyIdx++) {
        file_manager->bitsContainer[copyIdx] = GetElementsBit(file_manager->bufferData[file_manager->bufferIdx], BYTE_LENGTH - copyIdx - 1);
    }
}

void GetElementBits(ui8* elementBits, TRWFileBits* file_manager, size_t* inputSize) {
    for (int i = 0; i < BYTE_LENGTH; i++) {
        file_manager->bitsIdx += 1;
        if (IsBitsContainerOverflow(file_manager)) {
            UpdateBitsContainer(file_manager, inputSize);
        }
        elementBits[i] = file_manager->bitsContainer[file_manager->bitsIdx];
    }
}

void FillDataToHuffmanDecodingTreeLeaf(THuffmanTree* huffmanTree, TRWFileBits* file_manager, size_t* leavesCount, size_t* inputSize) {
    huffmanTree->left = NULL;
    huffmanTree->right = NULL;
    ui8 elementBits[BYTE_LENGTH];
    GetElementBits(elementBits, file_manager, inputSize);
    file_manager->bitsIdx++;
    if (IsBitsContainerOverflow(file_manager)) {
        UpdateBitsContainer(file_manager, inputSize);
    }
    ui8 element = GetElementByItsBinaryCode(elementBits);
    *leavesCount += 1;
    huffmanTree->character = element;
}

void GrowHuffmanDecodingTree(int dynamicBinCodesIdx, THuffmanTree* huffmanTree, TRWFileBits* file_manager,
    size_t* leavesCount, size_t* inputSize, ui8* dynamicSymbolsBinaryCode) {
    if (file_manager->bitsContainer[file_manager->bitsIdx] == 1) {
        huffmanTree->left = (THuffmanTree*)calloc(1, sizeof(THuffmanTree));
        huffmanTree->right = (THuffmanTree*)calloc(1, sizeof(THuffmanTree));
        file_manager->bitsIdx++;
        if (IsBitsContainerOverflow(file_manager)) {
            UpdateBitsContainer(file_manager, inputSize);
        }
        dynamicBinCodesIdx++;
        dynamicSymbolsBinaryCode[dynamicBinCodesIdx - 1] = 0;
        GrowHuffmanDecodingTree(dynamicBinCodesIdx, huffmanTree->left, file_manager,
            leavesCount, inputSize, dynamicSymbolsBinaryCode);
        dynamicSymbolsBinaryCode[dynamicBinCodesIdx - 1] = 1;
        GrowHuffmanDecodingTree(dynamicBinCodesIdx, huffmanTree->right, file_manager,
            leavesCount, inputSize, dynamicSymbolsBinaryCode);
    }
    else {
        FillDataToHuffmanDecodingTreeLeaf(huffmanTree, file_manager, leavesCount, inputSize);
    }
}

THuffmanTree* BuildHuffmanDecodingTree(int dynamicBinCodesIdx, THuffmanTree* huffmanTree, TRWFileBits* file_manager,
    size_t* leavesCount, size_t* inputSize, ui8* dynamicSymbolsBinaryCode) {
    huffmanTree = (THuffmanTree*)calloc(1, sizeof(THuffmanTree));
    if (file_manager->bitsContainer[file_manager->bitsIdx] == 0) {
        FillDataToHuffmanDecodingTreeLeaf(huffmanTree, file_manager, leavesCount, inputSize);
    }
    else {
        GrowHuffmanDecodingTree(dynamicBinCodesIdx, huffmanTree, file_manager, leavesCount,
            inputSize, dynamicSymbolsBinaryCode);
    }
    return huffmanTree;
}

bool IsVertexLeaf(const THuffmanTree* huffmanTree) {
    return huffmanTree->left == NULL && huffmanTree->right == NULL;
}

void FindNextDecodingTreesLeaf(THuffmanTree* huffmanTree, ui8* currentCode,
    TRWFileBits* file_manager, size_t* inputSize) {
    file_manager->bitsIdx = file_manager->bitsIdx + 1;
    if (IsBitsContainerOverflow(file_manager)) {
        UpdateBitsContainer(file_manager, inputSize);
    }
    if (IsVertexLeaf(huffmanTree)) {
        *currentCode = (ui8)huffmanTree->character;
    }
    else {
        if (file_manager->bitsContainer[file_manager->bitsIdx] == 0) {
            FindNextDecodingTreesLeaf(huffmanTree->left, currentCode, file_manager, inputSize);
        }
        else {
            FindNextDecodingTreesLeaf(huffmanTree->right, currentCode, file_manager, inputSize);
        }
    }
}

bool AreMissingBitsReached(const size_t inputSize, const char howManyBitsMissing, TRWFileBits* file_manager) {
    return file_manager->bufferIdx == inputSize - 1 &&
        file_manager->bitsIdx + howManyBitsMissing == BYTE_LENGTH && inputSize < BUFFER_SIZE;
}

void BitsContainerAutoUpdater(TRWFileBits* file_manager, size_t* inputSize){
    if (IsBitsContainerOverflow(file_manager)) {
        UpdateBitsContainer(file_manager, inputSize);
    }
}

void PrintNativeTextByHuffmanTree(const size_t leavesCount, const char howManyBitsMissing, THuffmanTree* huffmanTree,
    TRWFileBits* file_manager, size_t* inputSize) {
    BitsContainerAutoUpdater(file_manager, inputSize);
    while (FileDoesntEnd(file_manager->bufferIdx, *inputSize)) {
        ui8 code;
        if (file_manager->bitsContainer[file_manager->bitsIdx] == 0) {
            if (AreMissingBitsReached(*inputSize, howManyBitsMissing, file_manager)) {
                break;
            }
            if (leavesCount == 1) {
                code = (ui8)huffmanTree->character;
                fwrite(&code, sizeof(ui8), 1, file_manager->out);
                file_manager->bitsIdx++;
                BitsContainerAutoUpdater(file_manager, inputSize);
                continue;
            }
            FindNextDecodingTreesLeaf(huffmanTree->left, &code, file_manager, inputSize);
            fwrite(&code, sizeof(ui8), 1, file_manager->out);
        }
        else {
            FindNextDecodingTreesLeaf(huffmanTree->right, &code, file_manager, inputSize);
            fwrite(&code, sizeof(ui8), 1, file_manager->out);
        }
        BitsContainerAutoUpdater(file_manager, inputSize);
    }
}

void DeleteCodesTable(const size_t realSize, TCodesTable* codesTable) {
    for (size_t i = 0; i < realSize; i++) {
        free(codesTable[i].bits);
    }
    free(codesTable);
}

void CloseStreams(TRWFileBits* file_manager) {
    fclose(file_manager->in);
    fclose(file_manager->out);
    free(file_manager);
}

void _deleteHuffmanTree(THuffmanTree* tree) {
    if (tree->left) {
        _deleteHuffmanTree(tree->left);
        free(tree->left);
    }
    if (tree->right) {
        _deleteHuffmanTree(tree->right);
        free(tree->right);
    }
}

void DeleteHuffmanTree(THuffmanTree* huffmanTree) {
    if (huffmanTree) {
        _deleteHuffmanTree(huffmanTree);
        free(huffmanTree);
    }
}

void GetFirstBitsForDecodingTree(TRWFileBits* file_manager, size_t* inputSize) {
    file_manager->bufferIdx = 0;
    *inputSize = fread(file_manager->bufferData, sizeof(ui8), BUFFER_SIZE,
        file_manager->in);
    file_manager->bitsIdx = 0;
    for (int copyIdx = 0; copyIdx < BYTE_LENGTH; copyIdx++) {
        file_manager->bitsContainer[copyIdx] = GetElementsBit(file_manager->bufferData[file_manager->bufferIdx], BYTE_LENGTH - copyIdx - 1);
    }
}

void WriteInfoAboutEmptyFile(TRWFileBits* file_manager) {
    fseek(file_manager->out, 0, SEEK_SET);
    char emptyFileValue = FILE_EMPTY;
    fwrite(&emptyFileValue, sizeof(char), 1, file_manager->out);
}

bool IsNativeFileWasEmpty(char controlValue) {
    return controlValue == FILE_EMPTY;
}

void Zip(TRWFileBits* file_manager) {
    fseek(file_manager->out, 1, SEEK_CUR);
    size_t realTextElementsNumber = 0;
    THuffmanTree* priorityQueue = InputTextToPriorityQueue(file_manager, &realTextElementsNumber);
    if (realTextElementsNumber == 0) {
        WriteInfoAboutEmptyFile(file_manager);
        free(priorityQueue);
        return;
    }
    THuffmanTree* huffmanTree = BuildHuffmanCodingTree(realTextElementsNumber, priorityQueue);
    free(priorityQueue);
    TCodesTable* codesTable = (TCodesTable*)calloc(realTextElementsNumber, sizeof(TCodesTable));
    size_t codesTableIdx = 0;
    ui8 neededBits = 0;
    ui8 dynamicSymbolsBinaryCode[MAX_CHAR_NUM];
    CreateCodesTable(0, huffmanTree, &codesTableIdx, codesTable, dynamicSymbolsBinaryCode);
    WriteNewCodesToFile(realTextElementsNumber, huffmanTree, codesTable, file_manager, &neededBits);
    fseek(file_manager->out, 0, SEEK_SET);
    fwrite(&neededBits, sizeof(ui8), 1, file_manager->out);
    DeleteHuffmanTree(huffmanTree);
    DeleteCodesTable(realTextElementsNumber, codesTable);
}

void Unzip(TRWFileBits* file_manager){
    char howManyBitsAdded;
    if (!fread(&howManyBitsAdded, sizeof(char), 1, file_manager->in)) {
        return;
    }
    if (IsNativeFileWasEmpty(howManyBitsAdded)) {
        return;
    }
    THuffmanTree* huffmanTree = NULL;
    size_t currentInputSize;
    GetFirstBitsForDecodingTree(file_manager, &currentInputSize);
    size_t leavesCount = 0;
    ui8 dynamicSymbolsBinaryCode[MAX_CHAR_NUM];
    huffmanTree = BuildHuffmanDecodingTree(0, huffmanTree, file_manager, &leavesCount, &currentInputSize, dynamicSymbolsBinaryCode);
    PrintNativeTextByHuffmanTree(leavesCount, howManyBitsAdded, huffmanTree, file_manager, &currentInputSize);
    DeleteHuffmanTree(huffmanTree);
}
int main(void) {
    TRWFileBits* file_manager = (TRWFileBits*)calloc(1, sizeof(TRWFileBits));
    file_manager->in = fopen("in.txt", "rb");
    file_manager->out = fopen("out.txt", "wb");
    ui8* workingDirection = (ui8*)calloc(3, sizeof(ui8));
    if (fread(workingDirection, sizeof(ui8), 3, file_manager->in) != 3) {
        CloseStreams(file_manager);
        free(workingDirection);
        return EXIT_SUCCESS;
    }
    if (workingDirection[0] == ZIP_TEXT) {
        Zip(file_manager);
    }
    if (workingDirection[0] == UNZIP_TEXT) {
        Unzip(file_manager);
    }
    CloseStreams(file_manager);
    free(workingDirection);
    return EXIT_SUCCESS;
}
