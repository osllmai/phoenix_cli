import { cn } from "@/lib/utils"
import { IconSparkles } from "@tabler/icons-react"
import { useTheme } from "next-themes"
import Image from "next/image"
import { FC, HTMLAttributes } from "react"
import { AnthropicSVG } from "../icons/anthropic-svg"
import { GoogleSVG } from "../icons/google-svg"
import { OpenAISVG } from "../icons/openai-svg"

// Placeholder image import for local models (you can add your own placeholder)
import placeholderIcon from "@/public/providers/placeholder.png"

interface ModelIconProps extends HTMLAttributes<HTMLDivElement> {
  provider: string // Changed to string to handle custom/local models
  height: number
  width: number
}

export const ModelIcon: FC<ModelIconProps> = ({
                                                provider,
                                                height,
                                                width,
                                                ...props
                                              }) => {
  const { theme } = useTheme()

  switch (provider.toLowerCase()) {
    case "openai":
      return (
        <OpenAISVG
          className={cn(
            "rounded-sm bg-white p-1 text-black",
            props.className,
            theme === "dark" ? "bg-white" : "border-DEFAULT border-black"
          )}
          width={width}
          height={height}
        />
      )
    case "mistral":
      return (
        <Image
          className={cn(
            "rounded-sm p-1",
            theme === "dark" ? "bg-white" : "border-DEFAULT border-black"
          )}
          src={"/providers/mistral.png"} // Path to mistral logo if available
          alt="Mistral"
          width={width}
          height={height}
        />
      )
    case "groq":
      return (
        <Image
          className={cn(
            "rounded-sm p-0",
            theme === "dark" ? "bg-white" : "border-DEFAULT border-black"
          )}
          src={"/providers/groq.png"} // Path to groq logo if available
          alt="Groq"
          width={width}
          height={height}
        />
      )
    case "anthropic":
      return (
        <AnthropicSVG
          className={cn(
            "rounded-sm bg-white p-1 text-black",
            props.className,
            theme === "dark" ? "bg-white" : "border-DEFAULT border-black"
          )}
          width={width}
          height={height}
        />
      )
    case "google":
      return (
        <GoogleSVG
          className={cn(
            "rounded-sm bg-white p-1 text-black",
            props.className,
            theme === "dark" ? "bg-white" : "border-DEFAULT border-black"
          )}
          width={width}
          height={height}
        />
      )
    case "perplexity":
      return (
        <Image
          className={cn(
            "rounded-sm p-1",
            theme === "dark" ? "bg-white" : "border-DEFAULT border-black"
          )}
          src={"/providers/perplexity.png"} // Path to perplexity logo if available
          alt="Perplexity"
          width={width}
          height={height}
        />
      )

    // Custom Local Models - Placeholder for your local models
    default:
      return (
        <Image
          className={cn(
            "rounded-sm p-1",
            theme === "dark" ? "bg-white" : "border-DEFAULT border-black"
          )}
          src={placeholderIcon.src}
          alt="Local Model"
          width={width}
          height={height}
        />
      )
  }
}
