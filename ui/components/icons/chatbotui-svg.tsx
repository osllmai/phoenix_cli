import { FC } from "react"

interface ChatbotUISVGProps {
  theme: "dark" | "light"
  scale?: number
}

export const ChatbotUISVG: FC<ChatbotUISVGProps> = ({ theme, scale = 1 }) => {
  const iconSrc = `/icon-512x512.png`;
  const size = 512 * scale;

  return (
      <img
          src={iconSrc}
          alt="Chatbot UI Icon"
          width={size}
          height={size}
          style={{
            filter: theme === "dark" ? "invert(1)" : "none",
            display: "block"
          }}
      />
  );
}
