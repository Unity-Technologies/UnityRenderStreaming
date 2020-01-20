packageName=$1
sampleName=$2

echo Copying Packages
mkdir -p EmptyProjectForTest~/Packages/com.unity.renderstreaming
cp -r Packages/com.unity.renderstreaming EmptyProjectForTest~/Packages/com.unity.renderstreaming

echo Copying ${sampleName} Sample from ${packageName}
mkdir -p EmptyProjectForTest~/Assets/Samples/${sampleName}
cp -r Packages/${packageName}/Samples~/${sampleName} EmptyProjectForTest~/Assets/Samples/${sampleName}

echo Copying %sampleName% Finished
