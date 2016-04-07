#ifndef __OBB2D_H__
#define __OBB2D_H__

class Vector2
{
public: 
	typedef float data_type;
	
	Vector2() {
		m_Data[0] = m_Data[1] = 0.0f;
	}

	Vector2(const Vector2& other) {
		m_Data[0] = other.m_Data[0];
		m_Data[1] = other.m_Data[1];
	}

	Vector2(data_type x, data_type y) {
		m_Data[0] = x;
		m_Data[1] = y;
	}

	double dot(const Vector2& other) const {
		return other.m_Data[0] * m_Data[0] + other.m_Data[1] * m_Data[1];
	}

	double squaredLength() const {
		return sqrtf(m_Data[0]*m_Data[0] + m_Data[1]*m_Data[1]);
	}

	Vector2& operator/(data_type factor) {
		m_Data[0] /= factor;
		m_Data[1] /= factor;
		return *this;
	}

	Vector2& operator/=(data_type factor) {
		m_Data[0] /= factor;
		m_Data[1] /= factor;
		return *this;
	}

	Vector2& operator*(data_type factor) {
		m_Data[0] *= factor;
		m_Data[1] *= factor;
		return *this;
	}

	Vector2& operator*=(data_type factor) {
		m_Data[0] *= factor;
		m_Data[1] *= factor;
		return *this;
	}

	Vector2& operator+=(const Vector2& other) {
		m_Data[0] += other.m_Data[0];
		m_Data[1] += other.m_Data[1];
		return *this;
	}

	Vector2& operator=(const Vector2& other) {
		if (this==&other) {
			return *this;
		}
		m_Data[0] = other.m_Data[0];
		m_Data[1] = other.m_Data[1];
		return *this;
	}
	
	operator const data_type* () const {
		return &(m_Data[0]);
	}

	const data_type* data() const {
		return &(m_Data[0]);
	}

// 	friend static Vector2 operator-(const Vector2& first, const Vector2& second) {
// 		data_type x = first.m_Data[0] - second.m_Data[0];
// 		data_type y = first.m_Data[1] - second.m_Data[1];
// 		return Vector2(x, y);
// 	}
// 
// 	friend static Vector2 operator+(const Vector2& first, const Vector2& second) {
// 		data_type x = first.m_Data[0] + second.m_Data[0];
// 		data_type y = first.m_Data[1] + second.m_Data[1];
// 		return Vector2(x, y);
// 	}

//private:
	data_type m_Data[2];

};

static Vector2 operator-(const Vector2& first, const Vector2& second) {
	Vector2::data_type x = first.m_Data[0] - second.m_Data[0];
	Vector2::data_type y = first.m_Data[1] - second.m_Data[1];
	return Vector2(x, y);
}

static Vector2 operator+(const Vector2& first, const Vector2& second) {
	Vector2::data_type x = first.m_Data[0] + second.m_Data[0];
	Vector2::data_type y = first.m_Data[1] + second.m_Data[1];
	return Vector2(x, y);
}

class OBB2D {
private:
    /** Corners of the box, where 0 is the lower left. */
    Vector2         corner[4];

    /** Two edges of the box extended away from corner[0]. */
    Vector2         axis[2];

    /** origin[a] = corner[0].dot(axis[a]); */
    double          minProjLength[2];		// 原点 0点在两个轴上的投影
	double			maxProjLength[2];		// 2点在两个轴上的投影

    /** Returns true if other overlaps one dimension of this. */
    bool overlaps1Way(const OBB2D& other) const {
        for (int a = 0; a < 2; ++a) {

            double t = other.corner[0].dot(axis[a]);

            // Find the extent of box 2 on axis a
            double tMin = t;
            double tMax = t;

            for (int c = 1; c < 4; ++c) {
                t = other.corner[c].dot(axis[a]);

                if (t < tMin) {
                    tMin = t;
                } else if (t > tMax) {
                    tMax = t;
                }
            }

            // We have to subtract off the origin

            // See if [tMin, tMax] intersects [minProjLength, maxProjLength]
            if (tMin > maxProjLength[a] || tMax < minProjLength[a]) {
                // There was no intersection along this dimension;
                // the boxes cannot possibly overlap.
                return false;
            }
        }

        // There was no dimension along which there is no intersection.
        // Therefore the boxes overlap.
        return true;
    }


    /** Updates the axes after the corners move.  Assumes the
        corners actually form a rectangle. */
    void computeAxes() {
        axis[0] = corner[1] - corner[0]; 
        axis[1] = corner[3] - corner[0]; 

        // Make the length of each axis 1/edge length so we know any
        // dot product must be less than 1 to fall within the edge.

        for (int a = 0; a < 2; ++a) {
            axis[a] /= axis[a].squaredLength();
            
			minProjLength[a] = corner[0].dot(axis[a]);
			maxProjLength[a] = corner[2].dot(axis[a]);
        }
    }

public:

    OBB2D(const Vector2& center, const double w, const double h, double angle)
	{
        Vector2 X( cos(angle), sin(angle));
        Vector2 Y(-sin(angle), cos(angle));

        X *= w / 2;
        Y *= h / 2;

        corner[0] = center - X - Y;
        corner[1] = center + X - Y;
        corner[2] = center + X + Y;
        corner[3] = center - X + Y;

        computeAxes();
    }

	void updateAngle(const Vector2& center, const double w, const double h, double angle) {
		Vector2 X( cos(angle), sin(angle));
		Vector2 Y(-sin(angle), cos(angle));

		X *= w / 2;
		Y *= h / 2;

		corner[0] = center - X - Y;
		corner[1] = center + X - Y;
		corner[2] = center + X + Y;
		corner[3] = center - X + Y;

		computeAxes();
	}

    /** For testing purposes. */
    void moveTo(const Vector2& center) {
        Vector2 centroid = (corner[0] + corner[1] + corner[2] + corner[3]) / 4;

        Vector2 translation = center - centroid;

        for (int c = 0; c < 4; ++c) {
            corner[c] += translation;
        }

        computeAxes();
    }

    /** Returns true if the intersection of the boxes is non-empty. */
    bool overlaps(const OBB2D& other) const {
        return overlaps1Way(other) && other.overlaps1Way(*this);
    }

//     void render() const {
//         glBegin(GL_LINE_LOOP);
//             for (int c = 0; c < 5; ++c) {
//               glVertex2fv(corner[c & 3]);
//             }
//         glEnd();
//     }
};


#endif // __OBB2D_H__