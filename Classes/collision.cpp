#include "collision.h"

USING_NS_CC;

//////////////////////////////////////////////////////////////////////////
// box
BoxCollider::BoxCollider()
	: _pos(Vec2::ZERO)
	, _size(Size::ZERO)
	, _angle(0.0)
{

}

BoxCollider::BoxCollider(cocos2d::Vec2 pos, 
						 cocos2d::Size size,
						 float angle)
						 : _pos(pos)
						 , _size(size)
						 , _angle(angle)
{

}

bool BoxCollider::isCollision( BallCollider* pBall )
{
/*	Vec2 ball_pos = pBall->_pos; 
	float ball_radius = pBall->_radius;
	Vec2 speed = pBall->_speed;

	// vector from ball to block
	Vec2 distance = ball_pos - _pos;

	// rotate distance vector for block angle
	distance.rotate(Vec2::ZERO, CC_DEGREES_TO_RADIANS(_angle));

	// get project at x and y axis
	Vec2 px = distance.project(Vec2(1, 0));
	Vec2 py = distance.project(Vec2(0, 1));

	float half_width = _size.width / 2;
	float half_height = _size.height / 2;

	// 1 圆心在矩形边的投影都在矩形边长之外，
	// 此时圆只可能碰到矩形的角
	if (px.lengthSquared() > half_width * half_width &&
		py.lengthSquared() > half_height * half_height)
	{
		Vec2 vertexs[] = { Vec2(half_width, half_height), 
			Vec2(half_width, -half_height),
			Vec2(-half_width, half_height),
			Vec2(-half_width, -half_height)};

		for ( int i = 0; i < 4; ++ i)
		{
			if (vertexs[i].distanceSquared(distance) <= ball_radius * ball_radius)
			{
				Vec2 corner_pos = vertexs[i];
				corner_pos.rotate(Vec2::ZERO, CC_DEGREES_TO_RADIANS(-_angle));
				Vec2 newSpeed = BallBase::calcSpeedByCollision(speed, ball_pos, 
					_pos + corner_pos);
				pBall->setSpeed(newSpeed);
				return true;
			}
		}
	}
	// 2 圆心在矩形边的投影都在矩形边长之内，
	// 此时看圆点到矩形中心的距离是否小于半径与矩形边长一半的和
	else
	{
		float px_lenq = ball_radius + half_width;
		px_lenq *= px_lenq;
		float py_lenq = ball_radius + half_height;
		py_lenq *= py_lenq;

		if (px.lengthSquared() <= px_lenq &&
			py.lengthSquared() <= py_lenq)
		{
			Vec2 pTemp = px.lengthSquared() < py.lengthSquared() ? px : py;
			pTemp.rotate(Vec2::ZERO, CC_DEGREES_TO_RADIANS(-_angle));
			Vec2 newSpeed = BallBase::calcSpeedByCollision(speed, ball_pos, 
				pTemp + _pos);
			pBall->setSpeed(newSpeed);
			return true;
		}
	}
*/	return false;
}

//////////////////////////////////////////////////////////////////////////
// ball
BallCollider::BallCollider()
	: _pos(Vec2::ZERO)
	, _radius(0.0)
	, _speed(Vec2::ZERO)
{

}

BallCollider::BallCollider(cocos2d::Vec2 pos, 
						   float radius,
						   cocos2d::Vec2 speed)
						   : _pos(pos)
						   , _radius(radius)
						   , _speed(speed)
{

}

bool BallCollider::isCollision( BallCollider* pBall )
{
	bool br = false;
	float dist = _radius + pBall->_radius;
	br = _pos.getDistanceSq(pBall->_pos) <= dist * dist;
	if (br)
	{
		pBall->_speed = pBall->calcSpeed(this);
		_speed = calcSpeed(pBall);
	}
	return br;
}

// param is bullet
cocos2d::Vec2 BallCollider::calcSpeed( BallCollider* pBall )
{
	if (_speed.isZero())
		return Vec2::ZERO;

	Vec2 dist = pBall->_pos - _pos;
	Vec2 speed = _speed;
	Vec2 v = speed.project(dist);
	speed += -2 * v;
	return speed;
}

bool BallCollider::isCollision( BoxCollider* pBox )
{
	Vec2 ball_pos = _pos; 
	float ball_radius = _radius;
	Vec2 speed = _speed;
	Size sizeBox = pBox->_size;

	// vector from ball to block
	Vec2 distance = ball_pos - pBox->_pos;

	// rotate distance vector for block angle
	distance.rotate(Vec2::ZERO, CC_DEGREES_TO_RADIANS(pBox->_angle));

	// get project at x and y axis
	Vec2 px = distance.project(Vec2(1, 0));
	Vec2 py = distance.project(Vec2(0, 1));

	float half_width = sizeBox.width / 2;
	float half_height = sizeBox.height / 2;

	// 1 圆心在矩形边的投影都在矩形边长之外，
	// 此时圆只可能碰到矩形的角
	if (px.lengthSquared() > half_width * half_width &&
		py.lengthSquared() > half_height * half_height)
	{
		Vec2 vertexs[] = { Vec2(half_width, half_height), 
			Vec2(half_width, -half_height),
			Vec2(-half_width, half_height),
			Vec2(-half_width, -half_height)};

		for ( int i = 0; i < 4; ++ i)
		{
			if (vertexs[i].distanceSquared(distance) <= ball_radius * ball_radius)
			{
				Vec2 corner_pos = vertexs[i];
				corner_pos.rotate(Vec2::ZERO, CC_DEGREES_TO_RADIANS(-pBox->_angle));
				BallCollider tempBall(pBox->_pos + corner_pos, ball_radius, Vec2::ZERO);
				Vec2 newSpeed = calcSpeed(&tempBall);
				_speed = newSpeed;
				return true;
			}
		}
	}
	// 2 圆心在矩形边的投影都在矩形边长之内，
	// 此时看圆点到矩形中心的距离是否小于半径与矩形边长一半的和
	else
	{
		float px_lenq = ball_radius + half_width;
		px_lenq *= px_lenq;
		float py_lenq = ball_radius + half_height;
		py_lenq *= py_lenq;

		if (px.lengthSquared() <= px_lenq &&
			py.lengthSquared() <= py_lenq)
		{
			Vec2 pTemp = px.lengthSquared() < py.lengthSquared() ? px : py;
			pTemp.rotate(Vec2::ZERO, CC_DEGREES_TO_RADIANS(-pBox->_angle));
			BallCollider tempBall(pBox->_pos + pTemp, ball_radius, Vec2::ZERO);
			Vec2 newSpeed = calcSpeed(&tempBall);
			_speed = newSpeed;
			return true;
		}
	}

	return false;
}