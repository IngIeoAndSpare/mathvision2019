const POW_NUM = 2;

var originPoint = [],
    movePoint = [];

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

    },
    
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
