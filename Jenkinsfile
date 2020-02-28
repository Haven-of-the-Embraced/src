pipeline {
    agent {
        node {
            label 'master'
            customWorkspace '/home/havendev/src'
        }
    }

    stages {
        stage('Build') {
            steps {
                echo 'Doing a Clean Build...'
                sh '''
                make clean
                make
                '''
            }
        }
        stage('Test') {
            steps {
                echo 'Executing a Test Run...'
                dir('/home/havendev/area') {
                    sh '../src/rom 2002 testrun'
                }
            }
        }
    }
    post {
        always {
            script {
                def msg = "**Status:** " + currentBuild.currentResult.toLowerCase() + "\n"
                msg += "**Branch:** ${BRANCH_NAME}\n"
                msg += "**Changes:** \n"
                if (!currentBuild.changeSets.isEmpty()) {
                    currentBuild.changeSets.first().getLogs().each {
                        msg += "- `" + it.getCommitId().substring(0, 8) + "` *" + it.getComment().substring(0, it.getComment().length()-1) + "*\n"
                    }
                } else {
                    msg += "no changes for this run\n"
                }

                if (msg.length() > 1024) msg.take(msg.length() - 1024)

                withCredentials([string(credentialsId: 'DiscordWebhook', variable: 'discordWebhook')]) {
                    discordSend thumbnail: "https://havenmud.net/images/taskfailed.jpg", successful: currentBuild.resultIsBetterOrEqualTo('SUCCESS'), description: "${msg}", link: env.BUILD_URL, title: "HavenMUD:${branch} #${BUILD_NUMBER}", webhookURL: "${discordWebhook}"
                }
            }
        }
    }
}
