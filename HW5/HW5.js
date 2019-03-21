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

var Vector = (x, y, z) => {
    this.x = x;
    this.y = y;
    this.z = z;
}

var Point = (x, y, z) => {
    this.x = x;
    this.y = y;
    this.z = z;    
}

module.exports = {
    init : function () {
        let originPointArray = this.getPointArray(originPoint);
        let movePointArray = this.getPointArray(movePoint);

    },
    getPointArray : function (pointArray) {
        let pointArray = [];
        for (let pointItem of pointArray) {
            pointArray.push(
                new Point(pointItem[X], pointItem[Y], pointItem[Z])
            );
        }
        return pointArray;
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
     * get this -> target Normal vector
     * @param {Vector} targetVector  target vector. 
     */
    getNormalVector : function (targetVector) {
        
    },
    getVectorLength : function () {
        return Math.sqrt(
            Math.pow(this.x, POW_NUM) + Math.pow(this.y, POW_NUM) + Math.pow(this.z, POW_NUM)
        );
    },
    getCrossProduct : function (targetVector) {
        let x = (this.y * targetVector.z) - (this.z * targetVector.y),
            y = (this.z * targetVector.x) - (this.x * targetVector.z),
            z = (this.x * targetVector.y) - (this.y * targetVector.x);

        return new Vector(x, y, z);
    },
    getDotProduct : function (targetVector) {
        return (this.x * targetVector.x) + (this.y * targetVector.y) + (this.z * targetVector.z); 
    }
}
