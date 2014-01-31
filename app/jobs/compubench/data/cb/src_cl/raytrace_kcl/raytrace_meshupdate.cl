// per vertex skinned
__kernel void skinVertsToWorld(const uint vertexStart, const uint normalStart, const uint skinnedIndex,
							   __global const float4* verts, __global const float4* norms, __global const float4* tangs,
							   __global const float4* bone_weights, __global const uint4* bone_indices, __global const float16* bone_matrices,
							   __global float4* vertsW, __global float4* normsW, __global float4* tangsW)
{
	const int wiId = get_global_id(0);

	const float16 m0 = bone_weights[3*skinnedIndex + wiId].s0 * bone_matrices[ bone_indices[3*skinnedIndex + wiId].s0 ];
	const float16 m1 = bone_weights[3*skinnedIndex + wiId].s1 * bone_matrices[ bone_indices[3*skinnedIndex + wiId].s1 ];
	const float16 m2 = bone_weights[3*skinnedIndex + wiId].s2 * bone_matrices[ bone_indices[3*skinnedIndex + wiId].s2 ];
	const float16 m3 = bone_weights[3*skinnedIndex + wiId].s3 * bone_matrices[ bone_indices[3*skinnedIndex + wiId].s3 ];

	const float16 m = m0 + m1 + m2 + m3;

	float4 _vertsW = verts[3*vertexStart + wiId];
	float4 _normsW = norms[3*vertexStart + wiId];
	float4 _tangsW = tangs[3*vertexStart + wiId];

	_vertsW = MulMat4Vec4(&m, &_vertsW);
	_normsW = MulMat4Vec4(&m, &_normsW);
	_tangsW = MulMat4Vec4(&m, &_tangsW);

	vertsW[3*normalStart + wiId] = _vertsW;
	normsW[3*normalStart + wiId] = _normsW;
	tangsW[3*normalStart + wiId] = _tangsW;
}

// per vertex rigid
__kernel void rigidVertsToWorld(const uint vertexStart, const uint normalStart, const uint rigidIndex,
							   __global const float4* verts, __global const float4* norms, __global const float4* tangs,
							   __global const float16* rigid_matrices,
							   __global float4* vertsW, __global float4* normsW, __global float4* tangsW)
{
	const int wiId = get_global_id(0);

	const float16 m = rigid_matrices[ rigidIndex ];

	float4 _vertsW = verts[3*vertexStart + wiId];
	float4 _normsW = norms[3*vertexStart + wiId];
	float4 _tangsW = tangs[3*vertexStart + wiId];

	_vertsW = MulMat4Vec4(&m, &_vertsW);
	_normsW = MulMat4Vec4(&m, &_normsW);
	_tangsW = MulMat4Vec4(&m, &_tangsW);

	vertsW[3*normalStart + wiId] = _vertsW;
	normsW[3*normalStart + wiId] = _normsW;
	tangsW[3*normalStart + wiId] = _tangsW;
}

// per triangle
__kernel void worldVertsToTris(const uint normalStart, __global const float4* vertsW, __global CLAABB* aabbs, __global float4* morton)
{
	const int wiId = get_global_id(0);

	// load tri data
	const float4 vertexA = vertsW[3*normalStart + 3*wiId  ];
	const float4 vertexB = vertsW[3*normalStart + 3*wiId+1];
	const float4 vertexC = vertsW[3*normalStart + 3*wiId+2];
	
	// calculate tri aabb
	CLAABB triaabb;

	triaabb.min = min( vertexA, min( vertexB, vertexC ) );
	triaabb.max = max( vertexA, max( vertexB, vertexC ) );

	// calculate tri morton value - tri center
	// const float4 trimortonvalue = (vertexA + vertexB + vertexC) / 3.0f;

	// calculate tri morton value -  aabb center
	const float4 trimortonvalue = ( triaabb.min + triaabb.max ) * 0.5f;

	// save tri data
	aabbs[wiId] = triaabb;
	morton[wiId] = trimortonvalue;
}