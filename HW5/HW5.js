const mathModule = require('mathjs');

const POW_NUM = 2;
const X = 0,
      Y = 1,
      Z = 2;

var originPoint = [
        [-0.5, 0, 2.12132],
        [0.5, 0, 2.12132],
        [0.5, -0.707107, 2.828427],
        [0.5, 0.707107, 2.828427],
        [1,1,1]
    ],
    movePoint = [
        [1.363005, -0.427103, 2.339082],
        [1.748084, 0.437983, 2.017688],
        [2.636461, 0.184843, 2.40071],
        [0,0,0]
    ];

function Vector (x, y, z) {
    this.x = x;
    this.y = y;
    this.z = z;
}

function Point (x, y, z) {
    this.x = x;
    this.y = y;
    this.z = z;    
}

module.exports = {
    init : function () {
        let originPointArray = this.getPointArray(originPoint);
        let movePointArray = this.getPointArray(movePoint);

        // R1, R2를 먼저 값을 구한 후 P4에 적용하여 P4' 가 나오는지 확인.
        // ref : https://sjwd.tistory.com/34
        // get vector
        let oriFrontVector = originPointArray[0].getVector(originPointArray[1]),
            oriBackVector = originPointArray[0].getVector(originPointArray[2]);

        let moveFrontVector = movePointArray[0].getVector(movePointArray[1]),
            moveBackVector = movePointArray[0].getVector(movePointArray[2]);

        // get normal vector
        let oriNormalVector = oriFrontVector.getCrossProduct(oriBackVector),
            moveNormalVector = moveFrontVector.getCrossProduct(moveBackVector);

        // get rotation value ref : https://darkpgmr.tistory.com/81
        let r1CosValue = this.getCosValue(oriNormalVector, moveNormalVector);
        let r1SinValue = this.getSinValue(r1CosValue);

    },
    getPointArray : function (sourcePointArray) {
        let pointArray = [];
        for (let pointItem of sourcePointArray) {
            pointArray.push(
                new Point(pointItem[X], pointItem[Y], pointItem[Z])
            );
        }
        return pointArray;
    },
    /**
     * get cos value
     * @param {Vector} sourceVector 
     * @param {Vector} targetVector 
     */
    getCosValue : function (sourceVector, targetVector) {
        return (
            sourceVector.getDotProduct(targetVector) /
            sourceVector.getVectorLength() * targetVector.getVectorLength()
        )
    },
    /**
     * get sin value
     * @param {float} cosValue 
     */
    getSinValue : function (cosValue) {
        return Math.sqrt(1 - Math.pow(cosValue, POW_NUM));
    }

    
}

Point.prototype = {
    /**
     * return vector to this_point-target_point
     * @param {Point} targetPoint 
     */
    getVector: function (targetPoint) {
        return new Vector(
            targetPoint.x - this.x,
            targetPoint.y - this.y,
            targetPoint.z - this.z
        );
    },
    /**
     * return length 
     * @param {Point} targetPoint 
     */
    getLength: function (targetPoint) {
        return Math.sqrt(
            Math.pow(targetPoint.x - this.x, POW_NUM) +
            Math.pow(targetPoint.y - this.y, POW_NUM) +
            Math.pow(targetPoint.z - this.z, POW_NUM)
        );
    }
}

Vector.prototype = {
    /**
     * get vector length. 
     */
    getVectorLength : function () {
        return Math.sqrt(
            Math.pow(this.x, POW_NUM) + Math.pow(this.y, POW_NUM) + Math.pow(this.z, POW_NUM)
        );
    },
    /**
     * get Cross product. thisVector X targetVector
     * @param {Vector} targetVector 
     */
    getCrossProduct : function (targetVector) {
        let x = (this.y * targetVector.z) - (this.z * targetVector.y),
            y = (this.z * targetVector.x) - (this.x * targetVector.z),
            z = (this.x * targetVector.y) - (this.y * targetVector.x);

        return new Vector(x, y, z);
    },
    /**
     * get Dot product value. thisVector * targetVector
     * @param {Vector} targetVector 
     */
    getDotProduct : function (targetVector) {
        return (this.x * targetVector.x) + (this.y * targetVector.y) + (this.z * targetVector.z); 
    },
    /**
     * print vector x, y, z info
     */
    printVectorInfo : function () {
        console.log("x => " + this.x + "   y => " + this.y + "    z => " + this.z);
    },
    /**
     * convert vector to metrix
     * @param {Flag} flag  true : return column metrix  // false : return row metrix
     */
    convertVectorToArray : function (flag) {
        return (
            flag ? [this.x, this.y, this.z] : 
                    [[this.x, this.y, this.z]]
        );
    }
}
