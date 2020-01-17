packageName=$1
sampleName=$2

echo Copying Packages
cp -r Packages/com.unity.renderstreaming EmptyProjectForTest~/Packages/com.unity.renderstreaming

echo Copying ${sampleName} Sample from ${packageName}
cp -r Packages/${packageName}/Samples~/${sampleName} EmptyProjectForTest~/Assets/Samples/${sampleName}

echo Copying %sampleName% Finished
