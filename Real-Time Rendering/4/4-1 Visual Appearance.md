# 시각적 외향 (Visual Appearance)

## 광원 (Light Source)   
1. 방향 광원(directional light)   
2. 점 광원(point light)   
3. 집중 조명 광원(spot light)   

- Light Common Parameter (OpenGL, DirectX)
  - ambient, diffuse, specular, position
  - direction, cut-off, spot exponent (spot light)

## 물질 (Material)
- Material Parameter
  - ambient, diffuse, specular, shininess, emissive

## 조명 처리 (Lighting)
- 물체와 광원, 렌더링 될 객체 기하 구조와의 상호작용
- 색상, 텍스처, 투명성과 함께 사용

## 셰이딩 (Shading)
- 조명 효과의 계산으로부터 픽셀의 색상을 결정하는 과정
1. Flat Shading
- 개별 삼각형에 대해 삼각형 전체의 색상 값을 결정
    - 장점: 빠른 렌더링 속도
    - 단점: 낮은 화질

2. Gouraud Shading
- 삼각형의 정점에서 조명값 결정 후 표면 전체로 보간하는 방법
    - 장점: 적절한 속도, 부드러운 화질
    - 단점: 집중 조명광 효과 미비함, 하이라이트 잘 표시하지 못함, 마하밴드(Mach band) 효과 발생    
    ※ 조명 효과를 이용하는 텍스쳐로 부분 해소 가능

3. Phong Shading
- 법선 벡터는 삼각형 전체로 보간되고 색상값을 각각의 픽셀에서 계산하는 방법
    - 장점: 최고 화질, Gouraud shading 단점 회피
    - 단점: 비싼 비용, 정점별 조명 처리에 비해 복잡함
