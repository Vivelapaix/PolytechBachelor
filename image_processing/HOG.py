import cv2
import os
import glob
import numpy as np
import imutils
import matplotlib.pyplot as plt
from sklearn import svm
from imutils.object_detection import non_max_suppression as nms


def extractHOGFeatures(path, traindata, trainlabels, label):
    filenames = glob.iglob(os.path.join(path, '*'))
    for filename in filenames:
        image = cv2.imread(filename, cv2.IMREAD_GRAYSCALE)
        myhog = cv2.HOGDescriptor()
        feature = myhog.compute(image)
        feature = feature.transpose()
        traindata.append(feature)
        trainlabels.append(label)
    return traindata, trainlabels


def trainSVM(traindata, trainlabels):
    classifier = svm.SVC(kernel='linear', C=1.0)
    arr = np.array(traindata, 'float64')
    n, nx, ny = arr.shape
    d2_train_dataset = arr.reshape((n, nx * ny))
    classifier.fit(d2_train_dataset, trainlabels)
    return classifier


def resize(img, downscaleFactor):
    return cv2.resize(img, (int(img.shape[1] * (1 / downscaleFactor)),
                      int(img.shape[0] * (1 / downscaleFactor))), interpolation=cv2.INTER_AREA)


def pyramid(image, scale, minSize):
    yield image
    while True:
        image = resize(image, scale)
        if image.shape[0] < minSize[1] or image.shape[1] < minSize[0]:
           break
        yield image


def sliding_window(image, sizeOfWindow, sizeOfStep):
    for y in xrange(0, image.shape[0], sizeOfStep[1]):
        for x in xrange(0, image.shape[1], sizeOfStep[0]):
            yield (x, y, image[y: y + sizeOfWindow[1], x: x + sizeOfWindow[0]])


def detect(filename, classifier):

    im = cv2.imread(filename, cv2.IMREAD_GRAYSCALE)
    image = cv2.imread(filename)
    im = imutils.resize(im, width=min(400, im.shape[1]), height=min(400, im.shape[1]))
    image = imutils.resize(image, width=min(400, im.shape[1]), height=min(400, im.shape[1]))

    minSizeOfWindow = (64, 128)
    sizeOfStep = (5, 5)
    downscale = 1.25

    detections = []

    for resizedImage in pyramid(im, downscale, minSizeOfWindow):
        scale = float(im.shape[1]) / float(resizedImage.shape[1])
        if resizedImage.shape[0] < minSizeOfWindow[1] or resizedImage.shape[1] < minSizeOfWindow[0]:
            break
        for (x, y, partOfImage) in sliding_window(resizedImage, minSizeOfWindow, sizeOfStep):
            if partOfImage.shape[0] != minSizeOfWindow[1] or partOfImage.shape[1] != minSizeOfWindow[0]:
                continue

            myHOG = cv2.HOGDescriptor()
            feature = myHOG.compute(partOfImage)
            feature = feature.transpose()

            prediction = classifier.predict(feature)

            if prediction == 1 and classifier.decision_function(feature) > 0.5:
                detections.append(
                (
                    classifier.decision_function(feature),
                    int(x * (scale)),
                    int(y * (scale)),
                    int(minSizeOfWindow[0] * (scale)),
                    int(minSizeOfWindow[1] * (scale)),
                ))

    rects = np.array([[x, y, x + w, y + h] for (_, x, y, w, h) in detections])
    scr = [score[0] for (score, x, y, w, h) in detections]
    scr = np.array(scr)
    boxes = nms(rects, probs=scr, overlapThresh=0.25)

    for (x1, y1, x2, y2) in boxes:
        cv2.rectangle(image, (x1, y1), (x2, y2), (0, 255, 0), 2)

    plt.imshow(cv2.cvtColor(image, cv2.COLOR_BGR2RGB))
    plt.show()

def main():
    foldername = 'test'
    posImages = 'train/pos'
    negImages = 'train/neg'
    traindata = []
    trainlabels = []

    traindata, trainlabels = extractHOGFeatures(posImages, traindata, trainlabels, 1)
    traindata, trainlabels = extractHOGFeatures(negImages, traindata, trainlabels, 0)

    clf = trainSVM(traindata, trainlabels)

    filenames = glob.iglob(os.path.join(foldername, '*'))
    for filename in filenames:
        detect(filename, clf)


if __name__ == '__main__':
    main()
